/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: Marcelo
 *
 * Created on 13 de Abril de 2020, 17:34
 */

#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <fstream>
#include <sys/stat.h>
#include <unistd.h>
#include <chrono>
#include <ctime>   
#include <vector>

#define logName "log_speedtest.csv"

using namespace std;

/*
 * 
 */

typedef struct {
    string down;
    string up;
    string ping;
    string meuip;
    string host;
    string server;
    bool online;
    string date;
    time_t utime;
} speed;

time_t get_date() {
    auto end = std::chrono::system_clock::now();
    return std::chrono::system_clock::to_time_t(end);
    //return std::ctime(&end_time);
}

size_t split(const std::string &txt, std::vector<std::string> &strs, char ch) {
    size_t pos = txt.find(ch);
    size_t initialPos = 0;
    strs.clear();

    // Decompose statement
    while (pos != std::string::npos) {
        strs.push_back(txt.substr(initialPos, pos - initialPos));
        initialPos = pos + 1;

        pos = txt.find(ch, initialPos);
    }

    // Add the last one
    if (initialPos < std::min(pos, txt.size()) - initialPos + 1)
        strs.push_back(txt.substr(initialPos, std::min(pos, txt.size()) - initialPos + 1));


    return strs.size();
}

string get_host(string str) {
    int space = 0;
    int i = 0;
    while (space < 2 && i < str.size())
        if (str[i++] == ' ')
            space++;


    string host;
    while (i < str.size()) {
        if (str[i] == ('('))
            break;
        host += str[i++];
    }
    host.pop_back();
    return host;
}

string get_ip(string str) {
    int i = 0;
    while (i < str.size())
        if (str[i++] == '(')
            break;

    string ip;
    while (i < str.size()) {
        if (str[i] == (')'))
            break;
        ip += str[i++];
    }

    return ip;
}

string get_value(string str) {
    int i = 0;

    while (i < str.size())
        if (str[i++] == ':')
            break;
    i++;

    int j = i;
    while (j < str.size())
        if (str[j++] == ' ')
            break;

    return str.substr(i, j - i - 1);

}

speed get_speed(string resp) {
    speed test;
    vector<string>strv;
    split(resp, strv, '\n');

    if (strv.size() != 9) {
        test.online = false;
        test.down = -1;
        test.up = -1;
        return test;
    } else
        test.online = true;

    test.host = get_host(strv[1]);
    test.meuip = get_ip(strv[1]);
    test.ping = get_value(strv[4]);
    test.down = get_value(strv[6]);
    test.up = get_value(strv[8]);
    test.server = get_host(strv[4]);


    time_t now = get_date();
    test.date = std::ctime(&now);

    test.date.pop_back();
    test.utime = now;

    return test;

}

std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose) > pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

inline bool exists_test(const std::string & name) {
    ifstream f(name.c_str());
    return f.good();
}

void create_log(string arqname) {
    ofstream arq;
    arq.open(arqname, ios::trunc);
    arq << "Date;Online;Download/Mbit/s;Upload/Mbit/s;Ping/ms;Ip;Host;Server;unixtime\n";
    arq.close();

}

int main(int argc, char** argv) {
//    ifstream arqsample;
//    arqsample.open("sample.txt");
//
//    string sample;
//    while (!arqsample.eof())
//        sample += arqsample.get();
//    sample.pop_back();


    if (!exists_test(logName))
        create_log(logName);

    ofstream arq;
    arq.open(logName, ios::app);



    if (arq.is_open()) {
        speed resul = get_speed(exec("speedtest"));
        //speed resul = get_speed(sample);

        arq << resul.date << ";" << resul.online << ";" << resul.down << ";" <<
                resul.up << ";" << resul.ping << ";" << resul.meuip << ";" <<
                resul.host << ";" << resul.server << ";" << resul.utime << endl;
        arq.flush();

    } else
        return 1;








    return 0;

}

