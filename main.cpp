#include<iostream>
#include<fstream>
#include<string>
#include<optional>
#include<cctype>
#include<unordered_map>

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

    std::unordered_map<int, std::size_t>statusCodes;
    while(std::getline(file,line)){
        totalLines++;
        std::optional<LogRecord> result = logParser(line);

        if(result.has_value()){
            validRequests++;
            statusCodes[result.value().statusCode]++;
            std::cout<<"Method "<<result.value().method<<"\n";
            std::cout<<"Path "<<result.value().path<<"\n";
            std::cout<<"Status Code "<<result.value().statusCode<<"\n";
            std::cout<<"Bytes "<<result.value().bytes<<"\n";
        }else{
            malinformedRequests++;
        }
    }

    std::cout<<"Total Lines : "<<totalLines<<'\n';
    std::cout<<"Valid Requests : "<<validRequests<<'\n';
    std::cout<<"Malinformed Requests : "<<malinformedRequests<<'\n';


    std::cout << "\nStatus Codes\n";

    for(const auto& [status,count]:statusCodes){
        std::cout<<status<<":"<<count<<"\n";
    }

    return 0;
}