#include <iostream>
#include <ctime>
#include <thread>
#include <cpr/cpr.h>
#include <vector>
#include <time.h>
#include <sys/time.h>
#include "json.hpp"

using namespace std;
using namespace std::literals::chrono_literals;
using json = nlohmann::json;

string name;
string username;
string password;
string accessToken;
string str2;
string str;
string token;
bool available;
vector<std::thread> threads;

string getTime()
{
    timeval curTime;
    gettimeofday(&curTime, NULL);
    int milli = curTime.tv_usec / 1000;
    char buffer [80];
    strftime(buffer, 80, "%H:%M:%S", gmtime(&curTime.tv_sec));
    char currentTime[84] = "";
    sprintf(currentTime, "%s.%03d", buffer, milli);
    return currentTime;
}

void getName()
{
    while (true) {
        cpr::Response nameChangeRequest = cpr::Put(cpr::Url{"https://api.minecraftservices.com/minecraft/profile/name/" + name},
        cpr::Header{{"Authorization", "Bearer " + accessToken}});
        if (nameChangeRequest.status_code == 200) {
            cout << "[SUCCESS] Name Sniped" << "(" << nameChangeRequest.elapsed << ")" << '\n';
            break;
        } else {
            cout << "[ERROR] " << nameChangeRequest.status_code << "(" << nameChangeRequest.elapsed << ")" << '\n';
            break;
        }
    }
}

int main() {
    cout << "Enter Email Address: ";
    cin >> username;
    cout << "Enter Password: ";
    cin >> password;
    cout << "New Username: ";
    cin >> name;

    cout << "\033[2J\033[1;1H";
    /* Get accessToken */
    json accountLoginJson = {
        {"username", username},
        {"password", password}
    };
    /* Get the account setup to use Mojangs API */
    cpr::Response authenticationRequest = cpr::Post(cpr::Url{"https://authserver.mojang.com/authenticate"},
                      cpr::Body{accountLoginJson.dump()},
                      cpr::Header{{"Content-Type", "application/json"}});
    /* Parse authentication request to get access token */
    json accessTokenJson = json::parse(authenticationRequest.text);
    vector<nlohmann::json> authParse {accessTokenJson["accessToken"]};
    accessToken = authParse[0];

    /* Gets time till username is available */
    cpr::Response dropTimeRequest = cpr::Get(cpr::Url{"https://mojang-api.teun.lol/droptime/" + name});
    json dropTimeJson = json::parse(dropTimeRequest.text);
    vector<nlohmann::json> dropTimeParse {dropTimeJson["UTC"]};
    /* THIS IS MESSY AND WAS THROWN TOGETHER IN ABOUT 3 MINUTES. Does not work if the time is 
    ex. 16:00:00. Will fix that and make the inside of this if statement cleaner if I 
    come back to work on this project */
    if (!(dropTimeJson["error"] == "this name is not dropping or has already dropped")) {
        str = dropTimeParse[0];
        str.erase(std::remove(str.begin(),str.end(),'\"'),str.end());
        std::istringstream ss(str);
        std::string delimiter = " ";

        size_t pos = 0;
        std::string token;
        string token2;
        string token3;
        while ((pos = str.find(delimiter)) != std::string::npos) {
            token = str.substr(0, pos);
            str.erase(0, pos + delimiter.length());
        }
        string last;
        std::istringstream ss2(str);
        str = str + ":";
        getline(ss2, token, ':');
        getline(ss2, token2, ':');
        getline(ss2, token3, ':');
        int token4 = stoi(token3);
        if (token4 == 0) {
            int middle = stoi(token2);
            middle--;
            token2 = std::to_string(middle);
            last = "59";
        } else {
            token4--;
            last = std::to_string(token4);
        }
        str = token + ":" + token2 + ":" + last;
        
        str = str + ".775";
    } else {
        available = true;
    }
    
    /* Change the name on the Account */
    cout << "[SNIPING] " << name << '\n';
    cout << "[Current Time] " << getTime() << "\n";
    cout << "[Drop Time] " << str << '\n';
    cout << "Waiting for Drop Time..." << '\n';
    while (true) {
        if ((getTime() == str) /*|| available == true*/) {
            cout << "===REQUESTS STARTED===" << '\n';
            for(int i = 0; i < 10; ++i){
                threads.push_back(std::thread(getName));
            }
            for(auto& thread : threads){
                thread.join();
                std::this_thread::sleep_for(0.002s);
            }
            break;
        }
    }
}