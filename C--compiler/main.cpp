#include <dirent.h>
#include <fstream>
#include <iostream>
#include <sys/stat.h>
#include <vector>

#include "LexicalAnaly.cpp"
#include "SyntaxAnaly.cpp"

int main(int argc, char** argv)
{
    std::vector<std::string> files;

    if (argc != 1) {
        for (int i = 1; i < argc; ++i) {
            files.push_back(argv[i]);
        }
    } else {
        const char* testCasesDirectory = "testcases";

        DIR* dir;
        struct dirent* entry;
        struct stat fileStat;

        dir = opendir(testCasesDirectory);
        if (!dir) {
            std::cout << "运行目录下不存在testcases目录." << std::endl;
            return 0;
        }

        while ((entry = readdir(dir)) != nullptr) {
            std::string filePath = testCasesDirectory;
            filePath += "/";
            filePath += entry->d_name;

            if (stat(filePath.c_str(), &fileStat) == 0 && S_ISREG(fileStat.st_mode)) {
                std::string extension = entry->d_name;
                if (extension.length() > 3 && extension.substr(extension.length() - 3) == ".txt") {
                    files.push_back(filePath);
                }
            }
        }

        closedir(dir);
    }

    const char* outputDirectory = "outputs";

    if (mkdir(outputDirectory) != 0 && errno != EEXIST) {
        std::cout << "无法创建输出目录." << std::endl;
        return 0;
    }

    for (const auto& file : files) {
        std::ofstream writer;

        try {
            std::string output = "outputs/";
            size_t pos = file.find_last_of("/");
            if (pos != std::string::npos) {
                output += file.substr(pos + 1);
            } else {
                output += file;
            }
            output = output.substr(0, output.length() - 3);
            output += ".txt";
            writer.open(output);
        } catch (const std::exception& e) {
            std::cout << "构造ofstream时出现错误." << std::endl;
            return 0;
        }

        std::cout << "开始编译" << file << std::endl;
        LexicalAnaly laxical(file.c_str());

        try {
            std::cout << "词法分析:" << std::endl;
            laxical.analyse(writer);
        } catch (const std::exception& e) {
            std::cout << "读取源文件时发生错误." << std::endl;
            return 0;
        }

        const char* grammarFile = "grammar.txt";

        struct stat ruleStat;
        if (stat(grammarFile, &ruleStat) != 0) {
            std::cout << "运行目录下未找到文法文件grammar.txt" << std::endl;
            return 0;
        }

        SyntaxAnaly syntax(grammarFile);

        try {
            writer << "\n\n";
        } catch (const std::exception& e) {
            std::cout << "读写错误." << std::endl;
        }

        std::cout << "语法分析:" << std::endl;

        try {
            syntax.analize(laxical.getToken2String(), writer);
        } catch (const std::exception& e) {
            std::cout << "ofstream.write方法出现错误." << std::endl;
            return 0;
        }
        std::cout << '\n';

        writer.close();
    }

    return 0;
}
