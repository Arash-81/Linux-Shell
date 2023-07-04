#include<sys/wait.h>
#include<readline/readline.h>
#include<readline/history.h>
#include <fstream>
#include <signal.h>
#include <limits>
#include "iostream"

using namespace std;

void saveInput(char *buf) {
    ofstream file;
    file.open("history", ios::app);
    if (file.is_open()) {
        file << buf << endl;
        file.close();
    }
}

int getInput(char *str) {
    char *buf;
    buf = readline("$ ");
    if (strlen(buf) != 0) {
        add_history(buf);
        saveInput(buf);
        strcpy(str, buf);
        return 0;
    } else {
        return 1;
    }
}

void printDirectory() {
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    auto username = getenv("USER");
    cout << username << ": " << cwd;
}

void getNonComments(char address[]) {
    pid_t pid = fork();
    if (pid == -1) {
        cerr << "\nFailed forking child..";
        return;
    } else if (pid == 0) {
        ifstream file;
        file.open(address);
        if (!file) {
            cerr << "\033[1;31mInvalid Address\033[0m\n";
            file.close();
            return;
        }
        if (file.is_open()) {
            string str;
            while (!file.eof()) {
                getline(file, str);
                if (file.fail() && !file.eof()) {
                    cerr << "\033[1;31mInput content is not valid!\033[0m\n";
                    file.clear();
                    file.ignore(numeric_limits<streamsize>::max(), '\n');
                    return;
                }
                char *line = const_cast<char *>(str.c_str());
                line = strsep(&line, "#");
                if (strlen(line) != 0)
                    cout << line << endl;
            }
            file.close();
        }
        exit(0);
    } else {
        wait(nullptr);
        return;
    }
}

void getFirstWord(char address[]) {
    pid_t pid = fork();
    if (pid == -1) {
        cerr << "\nFailed forking child..";
        return;
    } else if (pid == 0) {
        char command[1024]{};
        strcat(command, "cut -d ' ' -f1 ");
        strcat(command, address);
        system(command);
        exit(0);
    } else {
        wait(nullptr);
        return;
    }
}

void removeWhiteSpaces(char address[]) {
    pid_t pid = fork();
    if (pid == -1) {
        cerr << "\nFailed forking child..";
        return;
    } else if (pid == 0) {
        ifstream file;
        file.open(address);
        if (!file) {
            cerr << "\033[1;31mInvalid Address\033[0m\n";
            file.close();
            return;
        }
        if (file.is_open()) {
            string str;
            while (!file.eof()) {
                file >> str;
                if (file.fail() && !file.eof()) {
                    cerr << "\033[1;31mInput content is not valid!\033[0m\n";
                    file.clear();
                    file.ignore(numeric_limits<streamsize>::max(), '\n');
                    return;
                }
                if (str.length() != 0)
                    cout << str;
            }
            cout << endl;
            file.close();
        }
        exit(0);
    } else {
        wait(nullptr);
        return;
    }
}

void getMostFrequent(char address[]) {
    pid_t pid = fork();
    if (pid == -1) {
        cerr << "\nFailed forking child..";
        return;
    } else if (pid == 0) {
        char command[1024]{};
        strcat(command, "sort ");
        strcat(command, address);
        strcat(command, " | uniq -c | sort -rn "
                        "| head -n1 | cut ""-d ' ' -f8");
        system(command);
        exit(0);
    } else {
        wait(nullptr);
        return;
    }
}

void getLineCount(char address[]) {
    pid_t pid = fork();
    if (pid == -1) {
        cerr << "\nFailed forking child..";
        return;
    } else if (pid == 0) {
        ifstream file;
        file.open(address);
        if (!file) {
            cerr << "\033[1;31mInvalid Address\033[0m\n";
            file.close();
            return;
        }
        int count{};
        if (file.is_open()) {
            string str;
            while (!file.eof()) {
                getline(file, str);
                count++;
            }
            file.close();
            cout << "Lines Count: " << count << endl;
        }
        exit(0);
    } else {
        wait(nullptr);
        return;
    }

}

void getFirstLines(char address[], int count = 10) {
    pid_t pid = fork();
    if (pid == -1) {
        cerr << "\nFailed forking child..";
        return;
    } else if (pid == 0) {
        ifstream file;
        file.open(address);
        if (!file) {
            cerr << "\033[1;31mInvalid Address\033[0m\n";
            file.close();
            return;
        }
        int i{};
        if (file.is_open()) {
            string str;
            while (!file.eof()) {
                if (i == count)
                    return;
                getline(file, str);
                if (file.fail() && !file.eof()) {
                    cerr << "\033[1;31mInput content is not valid!\033[0m\n";
                    file.clear();
                    file.ignore(numeric_limits<streamsize>::max(), '\n');
                    return;
                }
                cout << str << endl;
                i++;
            }
            file.close();
        }
        exit(0);
    } else {
        wait(nullptr);
        return;
    }
}

void execArgs(char **parsed) {
    pid_t pid = fork();

    if (pid == -1) {
        cerr << "\nFailed forking child..";
        return;
    } else if (pid == 0) {
        if (execvp(parsed[0], parsed) < 0) {
            cerr << "\033[1;31mCould not execute command...\033[0m\n";
        }
        exit(0);
    } else {
        wait(nullptr);
        return;
    }
}

void openHelp() {
    cout << endl << "***Linux Shell OS Project***"
         << endl << "List of Commands supported:"
         << endl << ">cd: Move to another directory"
         << endl << ">ls: Get the list of files"
         << endl << ">exit: Quit the shell"
         << endl << ">nocomment: Get the code excluding all comments"
         << endl << ">linecount: Get the number of lines in the file"
         << endl << ">firstlines: Get the 10 first lines in the file"
         << endl << ">firstword: Get the first word in a file"
         << endl << ">removespace: Get file content removing all whitespaces"
         << endl << ">mostfrequent: Get the most frequent word in the file"
         << endl << "all other general commands available in UNIX shell";
}

int miscCommandsHandler(char **parsed) {
    int switchArg = 0;
    char *cmdsList[] = {"exit", "cd",
                        "help", "nocomment",
                        "linecount", "firstlines",
                        "firstword", "removespace", "mostfrequent"};

    for (int i = 0; i < 9; i++) {
        if (strcmp(parsed[0], cmdsList[i]) == 0) {
            switchArg = i + 1;
            break;
        }
    }
    switch (switchArg) {
        case 1:
            cout << "\nGoodbye\n";
            exit(0);
        case 2:
            chdir(parsed[1]);
            return 1;
        case 3:
            openHelp();
            return 1;
        case 4:
            getNonComments(parsed[1]);
            return 1;
        case 5:
            getLineCount(parsed[1]);
            return 1;
        case 6:
            getFirstLines(parsed[1]);
            return 1;
        case 7:
            getFirstWord(parsed[1]);
            return 1;
        case 8:
            removeWhiteSpaces(parsed[1]);
            return 1;
        case 9:
            getMostFrequent(parsed[1]);
            return 1;
        default:
            break;
    }
    return 0;
}

void parseSpace(char *str, char **parsed) {
    for (int i = 0; i < 100; i++) {
        parsed[i] = strsep(&str, " ");

        if (parsed[i] == nullptr)
            break;
        if (strlen(parsed[i]) == 0)
            i--;
    }
}

int processInput(char *str, char **parsed) {
    parseSpace(str, parsed);

    if (miscCommandsHandler(parsed))
        return 0;
    else
        return 1;
}

void sigintHandler(int sig_num) {
    signal(SIGINT, sigintHandler);
    fflush(stdout);
    cout << endl;
    printDirectory();
}

int main() {
    char input[1024]{}, *parsed[100]{};
    int execFlag{};
    system("clear");
    signal(SIGINT, sigintHandler);

    while (true) {
        printDirectory();
        if (getInput(input))
            continue;
        execFlag = processInput(input, parsed);

        if (execFlag == 1)
            execArgs(parsed);
    }
}