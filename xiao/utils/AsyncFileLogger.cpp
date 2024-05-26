/**
 * @file   AsyncFileLogger.cpp
 * @author xiao guo
 * 
 *
 * @date   2024-5-26 
 */
#include <xiao/utils/AsyncFileLogger.h>
#include <xiao/utils/Utilities.h>
#include <functional>
#include <iostream>

BEGIN_NAMESPACE(xiao)
static constexpr std::chrono::seconds xLogFlushTimeout{ 1 };
static constexpr size_t xMemBufferSize{ 4 * 1024 * 1024 };
extern const char* strerror_tl(int savedErrno);
END_NAMESPACE(xiao)

using namespace xiao;

AsyncFileLogger::AsyncFileLogger()
	: logBufferPtr_(new std::string), nextBufferPtr_(new std::string)
{
	logBufferPtr_->reserve(xMemBufferSize);
	nextBufferPtr_->reserve(xMemBufferSize);
}

AsyncFileLogger::~AsyncFileLogger()
{
	stopFlag_ = true;
	if (threadPtr_)
	{
		cond_.notify_all();
		threadPtr_->join();
	}
	{
		std::lock_guard<std::mutex> guard_(mutex_);
		if (logBufferPtr_->length() > 0)
		{
			writeBuffers_.push(logBufferPtr_);
		}
		while (!writeBuffers_.empty())
		{
			StringPtr tmpPtr = (StringPtr&&)writeBuffers_.front();
			writeBuffers_.pop();
			writeLogToFile(tmpPtr);
		}
	}
}

void AsyncFileLogger::output(const char* msg, const uint64_t len)
{
	std::lock_guard<std::mutex> guard_(mutex_);
	if (len > xMemBufferSize)
		return;
	if (!logBufferPtr_)
	{
		logBufferPtr_ = std::make_shared<std::string>();
		logBufferPtr_->reserve(xMemBufferSize);
	}
	if (logBufferPtr_->capacity() - logBufferPtr_->length() < len)
	{
		swapBuffer();
		cond_.notify_one();
	}
	if (writeBuffers_.size() > 25) // 100M bytes logs in buffer
	{
		++lostCounter_;
		return;
	}

	if (lostCounter_ > 0)
	{
		char logErr[128];
		auto strlen =
			snprintf(logErr,
				sizeof(logErr),
				"%llu log information is lost\n",
				static_cast<long long unsigned int>(lostCounter_));
		lostCounter_ = 0;
		logBufferPtr_->append(logErr, strlen);
	}
	logBufferPtr_->append(msg, len);
}

void AsyncFileLogger::flush()
{
	std::lock_guard<std::mutex> guard_(mutex_);
	if (logBufferPtr_->length() > 0)
	{
		swapBuffer();
		cond_.notify_one();
	}
}

void AsyncFileLogger::writeLogToFile(const StringPtr buf)
{
	if (!loggerFilePtr_)
	{
		loggerFilePtr_ =
			std::unique_ptr<LoggerFile>(new LoggerFile(filePath_,
				fileBaseName_, fileExtName_, switchOnLimitOnly_, maxFiles_));
	}
	loggerFilePtr_->writeLog(buf);
	if (loggerFilePtr_->getLength() > sizeLimit_)
	{
		loggerFilePtr_->switchLog(true);
	}
}

void AsyncFileLogger::logThreadFunc()
{
#ifdef __linux__
	prctl(PR_SET_NAEM, "AsyncFileLogger");
#endif // __linux__
	while (!stopFlag_)
	{
		{
			std::unique_lock<std::mutex> lock(mutex_);
			while (writeBuffers_.size() == 0 && !stopFlag_)
			{
				if (cond_.wait_for(lock, xLogFlushTimeout) ==
					std::cv_status::timeout)
				{
					if (logBufferPtr_->length() > 0)
					{
						swapBuffer();
					}
					break;
				}
			}
			tmpBuffers_.swap(writeBuffers_);
		}
		while (!tmpBuffers_.empty())
		{
			StringPtr tmpPtr = (StringPtr&&)tmpBuffers_.front();
			tmpBuffers_.pop();
			writeLogToFile(tmpPtr);
			tmpPtr->clear();
			{
				std::unique_lock<std::mutex> lock(mutex_);
				nextBufferPtr_ = tmpPtr;
			}
		}
		if (loggerFilePtr_)
			loggerFilePtr_->flush();
	}
}

void AsyncFileLogger::startLogging()
{
	threadPtr_ = std::unique_ptr<std::thread>(
		new std::thread(std::bind(&AsyncFileLogger::logThreadFunc, this)));
}

AsyncFileLogger::LoggerFile::LoggerFile(const std::string& filePath,
	const std::string& fileBaseName,
	const std::string& fileExtName,
	bool switchOnLimitOnly,
	size_t maxFiles)
	: creationDate_(Date::date()),
	filePath_(filePath),
	fileBaseName_(fileBaseName),
	fileExtName_(fileExtName),
	switchOnLimitOnly_(switchOnLimitOnly),
	maxFiles_(maxFiles)
{
	open();

	if (maxFiles_ > 0)
	{
		initFilenameQueue();
	}
}

void AsyncFileLogger::LoggerFile::open()
{
	fileFullName_ = filePath_ + fileBaseName_ + fileExtName_;
#ifndef _MSC_VER
	fp_ = fopen(fileFullName_.c_str(), "a");
#else
	auto wFullName{ utils::toNativePath(fileFullName_) };
	fp_ = _wfsopen(wFullName.c_str(), L"a+", _SH_DENYWR);
#endif // !_MSC_VER
	if (fp_ == nullptr)
	{
		std::cout << strerror_tl(errno) << std::endl;
	}
}

uint64_t AsyncFileLogger::LoggerFile::fileSeq_{ 0 };
void AsyncFileLogger::LoggerFile::writeLog(const StringPtr buf)
{
	if (fp_)
	{
		fwrite(buf->c_str(), 1, buf->length(), fp_);
	}
}

void AsyncFileLogger::LoggerFile::flush()
{
	if (fp_)
	{
		fflush(fp_);
	}
}

uint64_t AsyncFileLogger::LoggerFile::getLength()
{
	if (fp_)
		return ftell(fp_);
	return 0;
}

void AsyncFileLogger::LoggerFile::switchLog(bool openNewOne)
{
	if (fp_)
	{
		fclose(fp_);
		fp_ = nullptr;

		char seq[12];
		snprintf(seq,
			sizeof(seq),
			".%06llu",
			static_cast<long long unsigned int>(fileSeq_ % 1000000));
		++fileSeq_;
		std::string newName =
			filePath_ + fileBaseName_ + "." +
			creationDate_.toCustomedFormattedString("%y%m%d-%H%M%S") +
			std::string(seq) + fileExtName_;
#if !defined(_WIN32) || defined(__MINGW32__)
		rename(fileFullName_.c_str(), newName.c_str());
#else
		auto wFullName{ utils::toNativePath(fileFullName_) };
		auto wNewName{ utils::toNativePath(newName) };
		_wrename(wFullName.c_str(), wNewName.c_str());
#endif
		if (maxFiles_ > 0)
		{
			filenameQueue_.push_back(newName);
			if (filenameQueue_.size() > maxFiles_)
			{
				deleteOldFiles();
			}
		}
		if (openNewOne)
			open();
	}
}

AsyncFileLogger::LoggerFile::~LoggerFile()
{
	if (!switchOnLimitOnly_)
		switchLog(false);
	if (fp_)
		fclose(fp_);
}

void AsyncFileLogger::LoggerFile::initFilenameQueue()
{
	if (maxFiles_ <= 0)
	{
		return;
	}
	// walk through the directory and file all files
#if !defined(_WIN32) || defined(__MINGW32__)
	DIR* dp;
	struct dirent* dirp;
	struct stat st;

	if ((dp = opendir(filePath_.c_str())) == nullptr)
	{
		fprintf(stderr,
			"Can't open dir %s: %s\n",
			filePath_.c_str(),
			strerror_tl(errno));
		return;
	}

	while ((dirp = readdir(dp)) != nullptr)
	{
		std::string name = dirp->d_name;
		// <base>.yymmdd-hhmmss.000000<ext>
		// NOTE: magic number 21: the length of middle part of generated name
		if (name.size() != fileBaseName_.size() + 21 + fileExtName_.size() ||
			name.compare(0, fileBaseName_.size(), fileBaseName_) != 0 ||
			name.compare(name.size() - fileExtName_.size(),
				fileExtName_.size(),
				fileExtName_) != 0)
		{
			continue;
		}
		std::string fullname = filePath_ + name;
		if (stat(fullname.c_str(), &st) == -1)
		{
			fprintf(stderr,
				"Can't stat file %s: %s\n",
				fullname.c_str(),
				strerror_tl(errno));
			continue;
		}
		if (!S_ISREG(st.st_mode))
		{
			continue;
		}
		filenameQueue_.push_back(fullname);
		std::push_heap(filenameQueue_.begin(),
			filenameQueue_.end(),
			std::greater<>());
		if (filenameQueue_.size() > maxFiles_)
		{
			std::pop_heap(filenameQueue_.begin(),
				filenameQueue_.end(),
				std::greater<>());
			auto fileToRemove = std::move(filenameQueue_.back());
			filenameQueue_.pop_back();
			remove(fileToRemove.c_str());
		}
	}
	closedir(dp);
#else
	// TODO: windows implementation
#endif
	std::sort(filenameQueue_.begin(), filenameQueue_.end(), std::less<>());
}

void AsyncFileLogger::LoggerFile::deleteOldFiles()
{
	while (filenameQueue_.size() > maxFiles_)
	{
		std::string filename = std::move(filenameQueue_.front());
		filenameQueue_.pop_front();

#if !defined(_WIN32) || defined(__MINGW32__)
		int r = remove(filename.c_str());
#else
		auto wName{ utils::toNativePath(filename) };
		int r = _wremove(wName.c_str());
#endif
		if (r != 0)
		{
			fprintf(stderr,
				"Failed to remove file %s: %s\n",
				filename.c_str(),
				strerror_tl(errno));
		}
	}
}

void AsyncFileLogger::swapBuffer()
{
	writeBuffers_.push(logBufferPtr_);
	if (nextBufferPtr_)
	{
		logBufferPtr_ = nextBufferPtr_;
		nextBufferPtr_.reset();
		logBufferPtr_->clear();
	}
	else
	{
		logBufferPtr_ = std::make_shared<std::string>();
		logBufferPtr_->reserve(xMemBufferSize);
	}
}
