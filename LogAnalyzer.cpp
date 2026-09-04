#include "LogAnalyzer.h"
#include<iostream>
#include<vector>
void LogAnalyzer::process(const LogRecord& record){
        totalLines++;
        validRequests++;
        
        statusCodes[record.statusCode]++;
        RequestByHTTP[record.method]++;
        RequestByEndpoint[record.path]++;
        
        totalBytes  += record.bytes;
        if(maxBytes < record.bytes) maxBytes=record.bytes;
        if(minBytes > record.bytes) minBytes=record.bytes;

        if(record.statusCode >= 200 && record.statusCode < 300) StatusCodeCategory["2xx"]++;
        else if(record.statusCode >= 300 && record.statusCode < 400) StatusCodeCategory["3xx"]++;
        else if(record.statusCode >= 400 && record.statusCode < 500) StatusCodeCategory["4xx"]++;
        else StatusCodeCategory["5xx"]++;
}

void LogAnalyzer::recordMalinformed(){
    totalLines++;
    malInformedRequests++;
}

void LogAnalyzer::printSummary(){
    if(validRequests == 0){
        std::cout<<"No Valid Requests \n";
        return;
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
    std::cout<<"Malinformed : "<<malInformedRequests<<"\n\n";
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
}