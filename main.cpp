#include<iostream>
#include<fstream>
#include<string>
#include "LogAnalyzer.h"
#include "LogParser.h"
#include "LogRecord.h"
int main(int argc , char* argv[]){
    //Check File Argument 
    if(argc < 2){
        std::cerr << "Error: Missing File Argument  \n";
        std::cout << "Usage : " << argv[0] << "\n";
        return 1;
    }
    //Open File 
    std::ifstream file(argv[1]);

    //Check for Error
    if(!file.is_open()){
        std::cerr<<"Error: Could not open the file\n";
        return 1;
    }
    LogAnalyzer analyzer;
    std::string line;

    //Parsing 
    while(std::getline(file,line)){
        std::optional<LogRecord> recordOpt = logParser(line);
        if(recordOpt.has_value()){
            analyzer.process(recordOpt.value());
        }else{
            analyzer.recordMalinformed();
        }
    }

    analyzer.printSummary();

    return 0;
}
