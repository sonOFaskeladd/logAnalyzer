#include<iostream>
#include<fstream>
#include<string>
#include<optional>
#include<cctype>
#include<unordered_map>
#include<algorithm>
struct LogRecord{
    std::string method;
    std::string path;
    int statusCode;
    std::size_t bytes;
};
std::optional<LogRecord>logParser(const std::string& line){
    LogRecord record;
    //Method 
    size_t pos = line.find('"');
    if(pos == std::string::npos) return std::nullopt;
    size_t closingPos = line.find('"',pos+1);
    if(closingPos == std::string::npos) return std::nullopt;

    size_t methodStart = pos+1;
    while(methodStart < line.size() && std::isspace(static_cast<unsigned char>(line[methodStart]))) methodStart++;
    size_t methodEnd = line.find(' ',methodStart);
    if(methodEnd==std::string::npos)return std::nullopt;
    std::string method = line.substr(methodStart,methodEnd-methodStart);
    record.method=method;
    
    //Path 
    size_t pathStart = methodEnd + 1;
    while(pathStart < line.size() && std::isspace(static_cast<unsigned char>(line[pathStart])))pathStart++;
    size_t pathEnd = line.find(' ',pathStart);
    if(pathEnd == std::string::npos) return std::nullopt;
    std::string path = line.substr(pathStart,pathEnd-pathStart);
    record.path = path;

    //Status Codes
    size_t statusStart = closingPos + 1;
    while(statusStart < line.size() && std::isspace(static_cast<unsigned char>(line[statusStart]))) statusStart++;
    size_t statusEnd = line.find(' ',statusStart);
    if(statusEnd == std::string::npos) return std::nullopt;
    std::string statusCode = line.substr(statusStart,statusEnd - statusStart);
    try{
        int status = std::stoi(statusCode);
        if(status < 100 || status > 599) return std::nullopt;
        record.statusCode = status;
    }catch(const std::invalid_argument&){
        return std::nullopt;
    }catch(const std::out_of_range&){
        return std::nullopt;
    }
    //Bytes
    size_t byteStart = statusEnd + 1;
    while(byteStart < line.size() && std::isspace(static_cast<unsigned char>(line[byteStart])))byteStart++;
    size_t byteEnd = line.size();
    std::string byte = line.substr(byteStart,byteEnd-byteStart);
    try{
        size_t bytes = std::stoi(byte);
        record.bytes = bytes;
    }catch(const std::invalid_argument&){
        return std::nullopt;
    }catch(const std::out_of_range&){
        return std::nullopt;
    }

    return record;
}
int main(int argc , char* argv[]){
    if(argc < 2){
        std::cerr << "Error: Missing File Argument  \n";
        std::cout << "Usage : " << argv[0] << "\n";
        return 1;
    }

    std::ifstream file(argv[1]);

    if(!file.is_open()){
        std::cerr<<"Error: Could not open the file\n";
        return 1;
    }

    std::string line;
    std::size_t totalLines = 0;
    std::size_t validRequests = 0;
    std::size_t malinformedRequests = 0;
    std::unordered_map<std::string, std::size_t> RequestByHTTP;
    std::unordered_map<std::string, std::size_t> RequestByEndpoint;
    std::unordered_map<std::string, std::size_t> StatusCodeCategory;
    std::unordered_map<int, std::size_t>statusCodes;
    std::size_t totalBytes = 0;
    std::size_t minBytes = INT_MAX;
    std::size_t maxBytes = 0;
    while(std::getline(file,line)){
        totalLines++;
        std::optional<LogRecord> result = logParser(line);

        if(result.has_value()){
            validRequests++;
            int statusCode = result.value().statusCode;
            statusCodes[statusCode]++;
            RequestByHTTP[result.value().method]++;
            RequestByEndpoint[result.value().path]++;
            std::size_t currentByte = result.value().bytes;
            totalBytes += currentByte;

            if(statusCode >= 200 && statusCode < 300) StatusCodeCategory["2xx"]++;
            else if(statusCode >= 300 && statusCode < 400) StatusCodeCategory["3xx"]++;
            else if(statusCode >= 400 && statusCode < 500) StatusCodeCategory["4xx"]++;
            else StatusCodeCategory["5xx"]++;
            
            //Calculate Max and Minimum Byte
            if(maxBytes < currentByte) maxBytes=currentByte;
            if(minBytes > currentByte) minBytes=currentByte;
        }else{
            malinformedRequests++;
        }
    }

    std::vector<std::pair<std::string,int>> vec(RequestByEndpoint.begin(),RequestByEndpoint.end());

    std::sort(vec.begin(),vec.end(),[](const auto& a, const auto& b){
        return a.second > b.second;
    });

    double errorRate = (double)((StatusCodeCategory["4xx"] + StatusCodeCategory["5xx"])/(double)validRequests)*100;

    std::cout<<"\n\n==========LOG ANALYSIS=========="<<"\n\n";
    std::cout<<"REQUESTS \n\n";
    std::cout<<"Total : "<<totalLines<<"\n";
    std::cout<<"Valid : "<<validRequests<<"\n";
    std::cout<<"Malinformed : "<<malinformedRequests<<"\n\n";
    std::cout<<"METHODS \n";
    for(const auto&[methods , count ]:RequestByHTTP){
        std::cout<<methods<<" : "<<count<<"\n";
    }
    std::cout<<"\nSTATUS \n";
    for(const auto&[status , count ]:StatusCodeCategory){
        std::cout<<status<<" : "<<count<<"\n";
    }
    std::cout<<"\nTOTAL ENDPOINTS \n";
    for(const auto&[path , count ]:RequestByEndpoint){
        std::cout<<path<<" : "<<count<<"\n";
    }
    std::cout<<"\nTOTAL BYTES : "<<totalBytes<<"\n";
    //Average Response Size 
    std::size_t averageBytes = totalBytes/validRequests;
    std::cout<<"\nAVERAGE RESPONSE SIZE : "<<averageBytes<<"\n";
    std::cout<<"\nMIN BYTES : "<<minBytes<<"\n";
    std::cout<<"\nMAX BYTES : "<<maxBytes<<"\n\n";
    std::cout<<"\nTOP 5 ENDPOINTS"<<"\n";
    int count = 0;
    for(const auto& pair : vec){
        if(count == 5) break;
        std::cout<<pair.first<<" : "<<pair.second<<"\n";
        count++;
    }
    std::cout<<"\nERROR RATE : "<<errorRate<<"%\n";
    return 0;
}
