#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>
#include <iterator>
#include <string>
#include <cmath>
#include <cstring>

using namespace std;

std::vector<std::string> machine_prog_lines;
std::vector<int> line_nbr;
std::vector<std::string> output;

std::string hex_conv(std::string value) {
    std::string conv_value;
    std::vector<std::pair<std::string, std::string>> hex_vec;
    hex_vec = {
        {"0000", "0"},{"0001","1"},{"0010","2"},{"0011","3"},
        {"0100","4"},{"0101","5"},{"0110","6"},{"0111","7"},
        {"1000","8"},{"1001","9"},{"1010","A"},{"1011","B"},
        {"1100","C"},{"1101","D"},{"1110","E"},{"1111","F"}
        };
    for(auto &i : hex_vec) {
        if (value == i.first) {
            conv_value = i.second;
        }
    }
    return conv_value;
}


std::string registers(std::string reg) {
    std::string instruction;
    std::for_each(reg.begin(), reg.end(), [](char & c) {
        c = ::tolower(c);
    });
    if (reg == "r0") {
        instruction = "00";
    } else if (reg == "r1") {
        instruction = "01";
    } else if (reg == "r2") {
        instruction = "10";
    } else {
        instruction = "11";
    }

    return instruction;
}


void two_op_inst(std::vector<std::string> inst_vec, 
    std::ofstream &FileWrite) {
    std::string instruction;
    std::string padding;
    std::string padded_inst;
    if (inst_vec.at(0) == "not") {
        instruction = "10";
    }
    std::string reg2 = registers(inst_vec.at(2));
    std::string first_op = instruction.append(reg2);
    padding = "00";
    std::string reg1 = registers(inst_vec.at(1));
    std::string second_op = padding.append(reg1);
    std::string final_val = hex_conv(first_op).append(hex_conv(second_op));
    FileWrite << final_val << std::endl;
    output.push_back(final_val);
}


void three_op_inst(std::vector<std::string> inst_vec,
    std::ofstream &FileWrite) {
    std::string instruction;
    std::string padding;
    std::string padded_inst;
    if (inst_vec.at(0) == "and") {
        instruction = "01";
    } else if (inst_vec.at(0) == "add") {
        instruction = "00";
    }
    std::string reg3 = registers(inst_vec.at(3));
    std::string reg2 = registers(inst_vec.at(2));
    std::string first_op = instruction.append(reg2);
    std::string second_op = reg3.append(registers(inst_vec.at(1)));
    std::string final_val = hex_conv(first_op).append(hex_conv(second_op));
    FileWrite << final_val << std::endl;
    output.push_back(final_val);
}


std::vector<int> binary(int nbr) {
    std::vector<int> vect(6,0);
    int copy = nbr;
    int i = 5;
    while(abs(copy) > 0){
        int digit = abs(copy) % 2;
        vect.at(i) = digit;
        copy = abs(copy) / 2;
        i--;
    }
    return vect;  
}


void one_op_inst(std::vector<std::string> inst_vec, 
    std::map<std::string, int> label_map, std::ofstream &FileWrite) {
    std::string instruction;
    std::string final_val;
    bool found = false;
    int label_value;
    for(auto &it: label_map) {
        if (inst_vec.at(1) == it.first) {
            found = true;
            label_value = it.second;
        }
    }
    if(found) {
        if (inst_vec.at(0) == "bnz") {
            instruction = "11";
        }

        std::vector<int> bin = binary(label_value);
        for(auto &i : bin) {
            instruction.append(std::to_string(i));
        }

        std::string first_op = instruction.substr(0,4);
        std::string second_op = instruction.substr(4,8);
        final_val = hex_conv(first_op).append(hex_conv(second_op));
    } else {
        final_val = "Label not found in the symbol table";
    }
    FileWrite << final_val << std::endl;
    output.push_back(final_val);

}




int main(int argc, char** argv) {
    if(argc < 3 || argc > 4) {
        std::cerr << "USAGE: fiscas <source file>" 
            << "<object file> [-l]" << std::endl;
        exit(0);
    } 
    std::ifstream file;
    std::ofstream FileWrite(argv[2]);
    std::string is_a_label;
    std::vector<std::string> lines;
    std::vector<std::string> vec;
    std::string label;
    std::string new_line;
    int line_count = 0;
    std::map<std::string, int> label_map;
    file.open(argv[1]);
    std::string line;
    while(std::getline(file, line)) {
        if (line.empty()){
            continue;
        }
        else {
            line = line.substr(0, line.find(";"));
            line_nbr.push_back(line_count + 1);
            lines.push_back(line);  
            if(!isspace(line.at(0))){
                if(line.find(":") == std::string::npos) {
                    label = line.substr(0, line.find(" "));
                    auto it = std::find(vec.begin(), vec.end(), label); 
                    if(it != vec.end()) {
                        continue;
                    } else {
                        vec.push_back(label);
                    }
                } else {
                    label = line.substr(0, line.find(":"));
                    auto it = std::find(vec.begin(), vec.end(), label); 
                    if(it != vec.end()) {
                        continue;
                    } else {
                        vec.push_back(label);
                    }
                }
                std::map<std::string, int>::iterator it = label_map.find(label);
                if(it != label_map.end()) {
                    continue;
                } else {
                    label_map.insert(std::make_pair(label,line_count));
                }
            }
            line_count++;  
        }
    }


    for(auto &line2: lines){
        std::string is_a_label = line2.substr(0, line2.find(" "));
        for(auto &e: label_map) {
            if(is_a_label == e.first) {
                size_t pos = line2.find(is_a_label);
                if (pos != std::string::npos) {
                    line2 = line2.erase(pos, is_a_label.length());
                }
            } else {
                line2 = line2.substr(line2.find(":") + 1);
            }
        }
    }

    std::vector<std::string> single_lines;
    const char* WhiteSpace = " \t\v\r\n";

    for(auto &line2: lines){
        std::string word;
        std::istringstream ss(line2);
        while(ss >> word){
            single_lines.push_back(word);
        }
        if(single_lines.size() == 2 && single_lines.at(0) == "bnz") {
            one_op_inst(single_lines, label_map, FileWrite);
        } else if(single_lines.size() == 3) {
            two_op_inst(single_lines, FileWrite);
        } else if(single_lines.size() == 4) {
            three_op_inst(single_lines, FileWrite);
        } else {
            std::string error = "Wrong number of instructions/operands";
            FileWrite << error << std::endl;
            output.push_back(error);
        }
        single_lines.clear();   
    }

    for(auto &line2: lines){
        std::size_t start = line2.find_first_not_of(WhiteSpace);
        line2.erase(0, start);
        machine_prog_lines.push_back(line2);
    }

    file.close();
    FileWrite.close();

    std::vector<std::string> str_lines;

    int dup = line_nbr.at(0);
    std::string str_dup = std::to_string(dup);
    int maxLen = str_dup.length();

    for(int i = 0; i < line_nbr.size(); i++) {
        int b = line_nbr.at(i);
        str_dup = to_string(b);
        if (maxLen < str_dup.length()){
            maxLen = str_dup.length();
        }
    }

    if (maxLen == 1){
        maxLen = 2;
    }

    for(int i = 0; i < line_nbr.size(); i++) {
        int b = line_nbr.at(i);
        std::string s_str = std::to_string(b);
        size_t size = line_nbr.size();
        std::string dup = std::to_string(line_nbr.at(size - 1));
        unsigned int zeros_append = dup.length() - s_str.length();
        if (zeros_append == 0){
            zeros_append = 1;
        }

        s_str.insert(0, maxLen - s_str.length(), '0');
        str_lines.push_back(s_str);

    }

    size_t MAXL = vec[0].length();
    for (auto &k: vec){
        if (k.length() > MAXL){
            MAXL = k.length();
        }
    }
    MAXL = MAXL + 3;

    if(argc == 4) {
        if(strcmp(argv[3],"-l") == 0) {
            std::cout << "*** LABEL LIST ***\n";
            for(auto &f: vec) {
                auto iter = label_map.find(f);
                char space[100];
                space[0] = '\0';
                memset(space, ' ', (MAXL - f.length()));
                space[MAXL - f.length()] = '\0'; 
                if (iter != label_map.end()) {
                    std::cout << iter->first << space << ":    " 
                        <<  iter->second << std::endl;
                }
            }
            std::cout << "*** MACHINE PROGRAM ***\n";
            for(int i = 0; i < str_lines.size(); i++) {
                std::cout << str_lines.at(i) << ":" << output.at(i)
                     << "   " << machine_prog_lines.at(i) << std::endl;
            }
        } else {
            std::cerr << "USAGE: fiscas <source file>" 
                << "<object file> [-l]" << std::endl;
            exit(0);
        }
    }  
}