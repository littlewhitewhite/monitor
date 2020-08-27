#pragma once
#undef max
#undef min
//#include <boost/lexical_cast.hpp>
//#include <boost/uuid/sha1.hpp> 
//#include <boost/archive/iterators/base64_from_binary.hpp>  
//#include <boost/archive/iterators/binary_from_base64.hpp>  
//#include <boost/archive/iterators/transform_width.hpp>  
//#include <boost/algorithm/hex.hpp>
//#include <boost/format.hpp> //for format  
#include <boost/date_time/posix_time/posix_time.hpp> 
#include <boost/locale/encoding.hpp>
#include <boost/uuid/sha1.hpp> 
#include <boost/archive/iterators/base64_from_binary.hpp>  
#include <boost/archive/iterators/binary_from_base64.hpp>  
#include <boost/archive/iterators/transform_width.hpp>  
//#include <boost/thread.hpp>

using boost::shared_ptr;
using namespace boost::uuids::detail;  
//using namespace boost::archive;
using namespace std;

class CBoostProc
{
public:
	CBoostProc(void);
	~CBoostProc(void);

	static bool Base64Encode(const std::string& sInput, std::string & sOutput) 
	{  
		typedef boost::archive::iterators::base64_from_binary<boost::archive::iterators::transform_width<std::string::const_iterator, 6, 8> > Base64EncodeIterator;  
		stringstream result;  
		copy(Base64EncodeIterator(sInput.begin()) , Base64EncodeIterator(sInput.end()), ostream_iterator<char>(result));  
		size_t equal_count = (3 - sInput.length() % 3) % 3;  
		for (size_t i = 0; i < equal_count; i++) {  
			result.put('=');  
		}  
		sOutput = result.str();  
		return sOutput.empty() == false;  
	} 
	static int64_t GetCurrentStamp64()
	{
		boost::posix_time::ptime epoch(boost::gregorian::date(1970, boost::gregorian::Jan, 1));
		//  boost::posix_time::microsec_clock::universal_time() - epoch;
		boost::posix_time::time_duration time_from_epoch = boost::posix_time::second_clock::universal_time() - epoch;

		//return time_from_epoch.total_microseconds();
		return time_from_epoch.total_seconds();
	}
	static std::string SimpleEncrypt(const std::string &sInput, int nKey)
	{
		static int C1 = 52845;
		static int C2 = 22719;
		std::string  sUtf = boost::locale::conv::to_utf<char>( sInput,"GBK"); 

		int nLen = sUtf.size();
		byte *bytes = new byte[nLen+1];
		ZeroMemory(bytes, nLen+1);
		byte *result = new byte[nLen+1];
		ZeroMemory(result, nLen+1);
		memcpy(bytes, sUtf.c_str(), nLen);

		for (int i = 0; i < nLen; i++)
		{
			result[i] = (byte)(bytes[i] ^ (nKey >> 8));
			nKey = ((byte)(result[i] + nKey)) * C1 + C2;
		}

		std::string sResult(  reinterpret_cast< char const* >(result) ) ;
		std::string sEncodResult("");
		Base64Encode(sResult, sEncodResult);
		delete []bytes;
		delete []result;
		return sEncodResult;
	}

	static std::string Boost_SHA1(const std::string &sTxt)
	{
		//SHA1加密
		sha1    sha;        // 声明摘要对象  
		sha.process_bytes(sTxt.c_str(), sTxt.size());  
		//sha.process_bytes(szMsg, strlen(szMsg));  
		//---------------------------  
		// 取出摘要中加密后的内容  
		//---------------------------  
		// 声明摘要的返回值  
		unsigned int digest[5];                 // unsigned int(4个字节) * 5个unsigned int = 20 个字节  
		sha.get_digest(digest);                 // 返回压缩后的20个字节的摘要内容  
		std::string sEncode("");
		char chTemp[9]={0};
		for (int i=0; i<5; ++i)  
		{  
			memset( chTemp, 0, 9 );
			//cout<< hex << digest[i];            // 以十六进制输出 
			std::sprintf(chTemp, "%08x", digest[i]);
			sEncode.append(chTemp);
		}  
		return sEncode;
	}
};

