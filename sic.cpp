//author abdelrahman rashed
//built using gnu c++23 minGW compiler
#include <bits/stdc++.h>
using namespace std;

map<string, int> iformat, icode;                                 //maps to store for each instuction its length and opcode

void load_formats(){                                             //loads instuction formats
    assert(freopen("opcode.txt", "r", stdin));                   //verify and open instruction text
    cin.clear();

    string iname;                                                //instuction name
    int ilen, inum;                                              //instuction length and number
    cin >> hex;                                                  //set input to hex
    while(cin >> iname >> ilen >> inum){                         //get instruction
        iformat[iname] = ilen;                 
        icode[iname] = inum;
        if(ilen == 3){
            iformat['+' + iname] = ilen + 1;                     //format 4 case
            icode['+' + iname] = inum;
        } 
    }
    cin >> dec;                                                  //reset input to dec
    fclose(stdin);                                               //close input stream
}
string pname;                                                    //program name
vector<string> labels, instructions, codes;               
vector<int> addresses;
map<string, int> laddr;                                          //address of label

void get_input(){                                                //get input file
    assert(freopen("input.txt", "r", stdin));                    //verify and open input text
    cin.clear();

    int cur;                                                     //current address
    string temp;                                                 //temporary string
    cin >> pname >> temp >> cur;                                 //get program name and starting address
    while(cin >> temp){
        addresses.push_back(cur);                                //add the address of the current instruction
        if(temp == "END"){                                       //program end
            instructions.push_back(temp);
            cin >> temp;
            codes.push_back(temp);
            labels.push_back("");
            break;
        }                               
        if(iformat.find(temp) == iformat.end()){                 //check for label
            labels.push_back(temp);                              //add label
            laddr[temp] = cur;                                   //add label address
            if(temp == "BASE"){                                  //check if base
                
                instructions.push_back(temp);
                cin >> temp;                                     
                codes.push_back(temp);
                continue;
            }
            cin >> temp;                                         //get instruction after label
        }
        else labels.push_back("");                               //place holer for no label

        instructions.push_back(temp);
        if(iformat.find(temp) == iformat.end()){                 //check for reserve/word/byte
            if(temp.substr(0, 3) == "RES"){                      //reserve condition
                int x;
                cin >> x;
                cur += x * (temp.back() == 'W' ? 3 : 1);         //add amount reserved to memory (mutliply by 3 if word)
                codes.push_back(to_string(x));                   //place holder
                continue;
            } 
            string s;
            cin >> s;
            if(temp == "WORD"){                                  //word condition
                cur += 3;                                        //add word size
                codes.push_back(s);                            
                continue;
            }                                                    //character condition
            if(s[0] == 'C') cur += s.size() - 3;                 //add character size if ascii
            else cur += s.size() - 3 >> 1;                       //add character size if hex
            codes.push_back(s);
            continue;
        }
        cur += iformat[temp];                                    //increment address by instuction length
        if(temp == "RSUB") codes.push_back("");                  //rsub condition
        else{
            cin >> temp;
            codes.push_back(temp);
        }
    }
    fclose(stdin);                                               //close input stream
}
void put_pass1(){                                                //put pass1
    freopen("pass1.txt", "w", stdout);                           //create(if needed) and open pass1 text
    cout.clear();

    cout << hex << uppercase;                                    //set output to uppercase hex
    cout << right << setfill('0') << setw(6) << addresses[0] << ' ';
    cout << left << setfill(' ') << setw(6) << pname << ' ';
    cout << setw(6) << "START" << ' ';
    cout << to_string(addresses[0]) << '\n';
    for(int i = 0; i < addresses.size(); i++){
        cout << right << setfill('0') << setw(6) << addresses[i] << ' '; //output address
        cout << left << setfill(' ') << setw(6) << labels[i] << ' ';     //output label
        cout << setw(6) << instructions[i] << ' ';                       //output instruction
        cout << codes[i] << '\n';
    }
    cout << dec << nouppercase << right;
    fclose(stdout);                                              //close output stream
}
void put_symbol_tabel(){                                         //put symbol table
    freopen("symbol table.txt", "w", stdout);                    //create(if needed) and open symbol table text
    cout.clear();

    cout << hex << uppercase;                                    //set output to uppercase hex
    for(auto& [label, addr] : laddr){
        cout << left << setfill(' ') << setw(6) << label << ' ';
        cout << right << setfill('0') << setw(6) << addr << '\n';
    }
    cout << dec << nouppercase << right;                         //reset output
    fclose(stdout);                                              //close output stream
}
vector<pair<int, int>> opcodes;                                  //instruction opcodes

void get_pass2(){                                                //get opcodes for pass2
    int reg[7]{};                                                // A, X, L, B, S, T, F
    map<char, int> rnum;                                         //number of each register;
    char nreg[] = {'A', 'X', 'L', 'B', 'S', 'T', 'F'};           //register of each number
    for(int i = 0; i < 7; i++) rnum[nreg[i]] = i;

    for(int i = 0; i < addresses.size(); i++){
        if(instructions[i] == "END"){
            opcodes.push_back({0, 0});
            break;
        }
        int x = 0, len = addresses[i + 1] - addresses[i];        //opcode and format length
        if(iformat.find(instructions[i]) == iformat.end()){      //check for format
            if(instructions[i] == "BASE"){                       //base condition
                if(laddr.find(codes[i]) != laddr.end()){
                    reg[3] = laddr[codes[i]];                    //set base to label address
                }
                else reg[3] = stoi(codes[i]);                    //set base to value
            }
            else if(instructions[i].find("RES") != string::npos){//reserve condition
                opcodes.push_back({0, 0});
                continue;
            }
            else if(instructions[i] == "WORD"){                  //word condition
                x |= stoi(codes[i]);
            }                                                    //byte condition
            else if(codes[i][0] == 'C'){                         //ascii condition
                string code = codes[i];
                code = code.substr(2, code.size() - 3);
                for(auto& j : code){
                    x <<= 8;
                    x |= j;
                }
            }
            else if(codes[i][0] == 'X'){                         //hex condition
                string code = codes[i];
                code = code.substr(2, code.size() - 3);
                for(auto& j : code){
                    x <<= 4;
                    x |= j - '0';
                    if(j >= 'A') x += '0' - 'A' + 10;
                }
            }
            opcodes.push_back({len, x});
            continue;
        }
        if(len == 1) x |= icode[instructions[i]];                //format 1 condition
        else if(len == 2){                                       //format 2 condition
            x |= icode[instructions[i]] << 8;
            x |= rnum[codes[i][0]] << 4;
            if(codes[i].size() > 1){                             //check for 2nd regisiter
                x |= rnum[codes[i].back()];
            }
        }
        else {                                                   //format 3/4 condition
            x |= icode[instructions[i]] << 16;
            string code = codes[i];
            int disp = 0;                                        //displacement
            x |= (code[0] != '#') << 17;                         //set bit n
            x |= (code[0] != '@') << 16;                         //set bit i
            if(code.empty()) goto here;
            if (code.find(",X") != string::npos){
                x |= 1 << 15;                                    //set bit x
                code = code.substr(0, code.size() - 2);
                disp -= reg[1];
            }
            if(code[0] == '#' && code[1] >= '0' && code[1] <= '9') code = code.substr(1);
            else{
                if(code[0] == '@' || code[0] == '#') code = code.substr(1);
                disp -= addresses[i + 1];
            } 
            if(len == 4){                                        //format 4 case
                x |= 1 << 12;                                    //set bit e
                x <<= 8;
                if(laddr.find(code) != laddr.end()){             //label condition
                    x |= laddr[code];  
                }
                else x |= stoi(code);                            //address/immediate condition
                goto here;
            }                                                    //format 3 case
            if(laddr.find(code) != laddr.end()){                 //label condition
                disp += laddr[code];
            }
            else x += stoi(code);                                //address/immediate
            if((x & (3 << 16)) != 1 << 16){                      //(in)direct condition
                if(disp < 2048 && disp >= -2048){                //pc relative
                    x |= 1 << 13;                                //set bit p
                }
                else{                                            //base relative
                    x |= 1 << 14;                                //set bit b
                    disp += addresses[i + 1] - reg[3];
                }
                x |= (1 << 11) - 1 & (disp);                     //set diplacement
                x |= (disp < 0) << 11;
            }
            else x |= disp;
            here:;
        }
        opcodes.push_back({len, x});                        
    } 
}
void put_pass2(){                                                //put pass2
    freopen("pass2.txt", "w", stdout);                           //create(if needed) and open pass2 text
    cout.clear();

    cout << hex << uppercase;                                    //set output to uppercase hex
    cout << right << setfill('0') << setw(6) << addresses[0] << ' ';
    cout << left << setfill(' ') << setw(6) << pname << ' ';
    cout << setw(6) << "START" << ' ' << setw(8) << "" << ' ';
    cout << to_string(addresses[0]) << '\n';
    for(int i = 0; i < addresses.size(); i++){
        auto [len, val] = opcodes[i];                            //opcode length and value
        cout << right << setfill('0') << setw(6) << addresses[i] << ' '; 
        cout << left << setfill(' ') << setw(6) << labels[i] << ' ';   
        cout << setw(6) << instructions[i] << ' ';                
        cout << setw(8) << codes[i] << ' ';     
        if(len) cout << right << setfill('0') << setw(2 * len) << val;
        cout << '\n';
    }
    cout << dec << nouppercase << right;
    fclose(stdout);                                              //close output stream
}
void put_hte_record(){                                           //put hte record
    freopen("hte.txt", "w", stdout);                             //create(if needed) and open hte text
    cout.clear();

    cout << hex << uppercase;                                    //set output to uppercase hex
    cout << "H^" << pname << '^' << addresses[0] << '^' << addresses.back() << '\n';//header
    for(int i = 0; i < addresses.size() - 1; i++){
        int j = i;
        while(j + 1 < addresses.size() && addresses[j + 1] - addresses[i] <= 30){
            if(instructions[j].find("RES") != string::npos) break;//reserve condition
            j++;
        }
        if(i != j){
            cout << "T^" << addresses[i] << '^' << addresses[j] - addresses[i] << '^';//text
            while(i != j){
                if(instructions[i++] == "BASE") continue;       
                cout << opcodes[i - 1].second;
            }
            cout << '\n';
            i--;
        }
    }
    cout << "E^" << addresses[0] << '\n';                         //end
    cout << dec << nouppercase << right;
    fclose(stdout);                                               //close output stream
}
int main(){
    load_formats();
    get_input();

    put_pass1();
    put_symbol_tabel();

    get_pass2();
    put_pass2();

    put_hte_record();
}