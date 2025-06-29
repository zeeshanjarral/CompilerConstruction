#include <iostream>
#include <fstream>
#include <cctype>
#include <cstring>
using namespace std;

const int MAX_TOKENS = 1000;
const int MAX_ERRORS = 500;

struct Token {
    string lexeme;
    string type;
};

class LexicalAnalyzer {
private:
    string sourceCode;
    Token tokens[MAX_TOKENS];
    int tokenCount;
    string errors[MAX_ERRORS];
    int errorCount;
    
    bool isKeyword(const string& str) {
        // List of keywords as per specification
        string keywordList[] = {
            "loop", "agar", "magar", "asm", "else", "new", "this", "auto",
            "enum", "operator", "throw", "bool", "explicit", "private", "true",
            "break", "export", "protected", "try", "case", "extern", "public",
            "typedef", "catch", "false", "register", "typeid", "char", "float",
            "typename", "class", "for", "return", "union", "const", "friend",
            "short", "unsigned", "goto", "signed", "using", "continue", "if",
            "sizeof", "virtual", "default", "inline", "static", "void", "delete",
            "int", "volatile", "do", "long", "struct", "double", "mutable",
            "switch", "while", "namespace"
        };
        int nKeywords = sizeof(keywordList) / sizeof(keywordList[0]);
        for (int i = 0; i < nKeywords; ++i) {
            if (str == keywordList[i]) {
                return true;
            }
        }
        return false;
    }
    
    void addToken(const string& lex, const string& type) {
        if (tokenCount < MAX_TOKENS) {
            tokens[tokenCount].lexeme = lex;
            tokens[tokenCount].type = type;
            tokenCount++;
        }
    }
    
    void addError(const string& lex) {
        if (errorCount < MAX_ERRORS) {
            errors[errorCount++] = lex;
        }
    }
    
public:
    LexicalAnalyzer(): tokenCount(0), errorCount(0) {}
    
    bool loadFromFile(const string& filename) {
        ifstream fin(filename.c_str());
        if (!fin) {
            return false;
        }
        sourceCode.clear();
        string line;
        while (getline(fin, line)) {
            sourceCode += line;
            sourceCode.push_back('\n');  // keep newlines
        }
        fin.close();
        return true;
    }
    
    void setSourceCode(const string& src) {
        sourceCode = src;
    }
    
    void analyze() {
        tokenCount = 0;
        errorCount = 0;
        int n = sourceCode.size();
        int i = 0;
        
        while (i < n) {
            char c = sourceCode[i];
            // 1. Skip whitespace
            if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
                i++;
                continue;
            }
            // 2. Skip comments
            if (c == '/' && i + 1 < n && sourceCode[i+1] == '/') {
                // single-line comment
                i += 2;
                while (i < n && sourceCode[i] != '\n') {
                    i++;
                }
                continue;
            }
            if (c == '/' && i + 1 < n && sourceCode[i+1] == '*') {
                // multi-line comment
                i += 2;
                bool closed = false;
                while (i < n) {
                    if (sourceCode[i] == '*' && i + 1 < n && sourceCode[i+1] == '/') {
                        i += 2;
                        closed = true;
                        break;
                    }
                    i++;
                }
                if (!closed) {
                    addError("/*... (unterminated comment)");
                }
                continue;
            }
            // 3. Identifier or Keyword
            if (isalpha(c) || c == '_') {
                string lexeme;
                bool underscoreSeen = false;
                while (i < n && (isalpha(sourceCode[i]) || isdigit(sourceCode[i]) || sourceCode[i] == '_')) {
                    char ch = sourceCode[i];
                    lexeme.push_back(ch);
                    if (ch == '_') underscoreSeen = true;
                    i++;
                }
                if (isKeyword(lexeme)) {
                    addToken(lexeme, "Keyword");
                } else if (underscoreSeen) {
                    addToken(lexeme, "Identifier");
                } else {
                    addError(lexeme);
                }
                continue;
            }
            // 4. Number (integer, decimal, or exponent)
            if (isdigit(c) || ((c == '+' || c == '-') && i+1 < n && isdigit(sourceCode[i+1]))) {
                string lexeme;
                int state = 0;
                bool numberValid = true;
                while (i < n && numberValid) {
                    char ch = sourceCode[i];
                    switch (state) {
                        case 0:  // start
                            if (ch == '+' || ch == '-') {
                                lexeme.push_back(ch);
                                state = 1;
                                i++;
                            } else if (isdigit(ch)) {
                                lexeme.push_back(ch);
                                state = 2;
                                i++;
                            } else {
                                numberValid = false;
                            }
                            break;
                        case 1:  // sign read, need a digit
                            if (isdigit(ch)) {
                                lexeme.push_back(ch);
                                state = 2;
                                i++;
                            } else {
                                numberValid = false;
                            }
                            break;
                        case 2:  // integer part
                            if (isdigit(ch)) {
                                lexeme.push_back(ch);
                                // stay in state 2
                                i++;
                            } else if (ch == '.') {
                                lexeme.push_back(ch);
                                state = 3;
                                i++;
                            } else if (ch == 'E' || ch == 'e') {
                                lexeme.push_back(ch);
                                state = 5;
                                i++;
                            } else {
                                // token ends as integer
                                goto END_NUM;  // break out to finalize
                            }
                            break;
                        case 3:  // decimal point seen, expect fraction digit
                            if (isdigit(ch)) {
                                lexeme.push_back(ch);
                                state = 4;
                                i++;
                            } else {
                                numberValid = false;
                            }
                            break;
                        case 4:  // fractional part
                            if (isdigit(ch)) {
                                lexeme.push_back(ch);
                                i++;
                            } else if (ch == 'E' || ch == 'e') {
                                lexeme.push_back(ch);
                                state = 5;
                                i++;
                            } else {
                                // token ends as decimal
                                goto END_NUM;
                            }
                            break;
                        case 5:  // exponent marker seen
                            if (ch == '+' || ch == '-') {
                                lexeme.push_back(ch);
                                state = 6;
                                i++;
                            } else if (isdigit(ch)) {
                                lexeme.push_back(ch);
                                state = 7;
                                i++;
                            } else {
                                numberValid = false;
                            }
                            break;
                        case 6:  // exponent sign seen, expect digit
                            if (isdigit(ch)) {
                                lexeme.push_back(ch);
                                state = 7;
                                i++;
                            } else {
                                numberValid = false;
                            }
                            break;
                        case 7:  // exponent digits
                            if (isdigit(ch)) {
                                lexeme.push_back(ch);
                                i++;
                            } else {
                                // token ends after exponent
                                goto END_NUM;
                            }
                            break;
                    }
                }
                END_NUM:
                // Check if ended in a valid accepting state
                if (numberValid && (state == 2 || state == 4 || state == 7)) {
                    addToken(lexeme, "Number");
                } else {
                    addError(lexeme);
                }
                continue;
            }
            // 5. Operators and Punctuation
            // Multi-character operators handled with lookahead
            string opLex;
            switch (c) {
                case '!':
                    if (i+1 < n && sourceCode[i+1] == '=') {
                        opLex = "!=";
                        i += 2;
                        addToken(opLex, "Operator");
                    } else {
                        opLex = "!";
                        i++;
                        addError(opLex);
                    }
                    break;
                case '=':
                    if (i+1 < n && sourceCode[i+1] == '=') {
                        opLex = "=="; i += 2; addToken(opLex, "Operator");
                    } else if (i+1 < n && sourceCode[i+1] == '+') {
                        opLex = "=+"; i += 2; addToken(opLex, "Operator");
                    } else if (i+1 < n && sourceCode[i+1] == '>') {
                        opLex = "=>"; i += 2; addToken(opLex, "Operator");
                    } else if (i+1 < n && sourceCode[i+1] == '<') {
                        opLex = "=<"; i += 2; addToken(opLex, "Operator");
                    } else if (i+1 < n && sourceCode[i+1] == ':' && i+2 < n && sourceCode[i+2] == '=') {
                        opLex = "=:="; i += 3; addToken(opLex, "Operator");
                    } else {
                        opLex = "="; i++; addError(opLex);
                    }
                    break;
                case '<':
                    if (i+1 < n && sourceCode[i+1] == '>') {
                        opLex = "<>"; i += 2; addToken(opLex, "Operator");
                    } else {
                        // treat single '<' as punctuation (opening angle bracket)
                        opLex = "<"; i++; addToken(opLex, "Punctuation");
                    }
                    break;
                case '>':
                    if (i+1 < n && sourceCode[i+1] == '>') {
                        opLex = ">>"; i += 2; addToken(opLex, "Operator");
                    } else {
                        opLex = ">"; i++; addToken(opLex, "Punctuation");
                    }
                    break;
                case '+':
                    if (i+1 < n && sourceCode[i+1] == '+') {
                        opLex = "++"; i += 2; addToken(opLex, "Operator");
                    } else {
                        opLex = "+"; i++; addToken(opLex, "Operator");
                    }
                    break;
                case '-':
                    if (i+1 < n && sourceCode[i+1] == '-') {
                        opLex = "--"; i += 2; addToken(opLex, "Operator");
                    } else {
                        opLex = "-"; i++; addToken(opLex, "Operator");
                    }
                    break;
                case '&':
                    if (i+1 < n && sourceCode[i+1] == '&') {
                        opLex = "&&"; i += 2; addToken(opLex, "Operator");
                    } else {
                        opLex = "&"; i++; addError(opLex);
                    }
                    break;
                case '|':
                    if (i+1 < n && sourceCode[i+1] == '|') {
                        opLex = "||"; i += 2; addToken(opLex, "Operator");
                    } else {
                        opLex = "|"; i++; addError(opLex);
                    }
                    break;
                case '*':
                    opLex = "*"; i++; addToken(opLex, "Operator");
                    break;
                case '/':
                    opLex = "/"; i++; addToken(opLex, "Operator");
                    break;
                case '%':
                    opLex = "%"; i++; addToken(opLex, "Operator");
                    break;
                case ':':
                    if (i+1 < n && sourceCode[i+1] == ':') {
                        opLex = "::"; i += 2; addToken(opLex, "Operator");
                    } else {
                        opLex = ":"; i++; addToken(opLex, "Operator");
                    }
                    break;
                case '{': case '}': case '[': case ']': case '(': case ')': case ',': case ';':
                    // punctuation tokens
                    opLex = string(1, c);
                    i++;
                    addToken(opLex, "Punctuation");
                    break;
                default:
                    // any other character is unrecognized
                    opLex = string(1, c);
                    i++;
                    addError(opLex);
                    break;
            }
            // continue the while loop after handling operator/punctuation
        }
    }
    
    // Utility methods to retrieve or display the results:
    void printAllTokens() {
        for (int j = 0; j < tokenCount; ++j) {
            cout << tokens[j].lexeme << " -> " << tokens[j].type << endl;
        }
        if (tokenCount == 0) {
            cout << "(No tokens)" << endl;
        }
    }
    
    void viewTokensByType(int typeOption) {
        string typeName;
        switch(typeOption) {
            case 1: typeName = "Identifier"; break;
            case 2: typeName = "Number"; break;
            case 3: typeName = "Operator"; break;
            case 4: typeName = "Punctuation"; break;
            case 5: typeName = "Keyword"; break;
            default: return;
        }
        bool found = false;
        cout << typeName << " Tokens:" << endl;
        for (int j = 0; j < tokenCount; ++j) {
            if (tokens[j].type == typeName) {
                cout << tokens[j].lexeme << endl;
                found = true;
            }
        }
        if (!found) {
            cout << "(None)" << endl;
        }
    }
    
    void printErrors() {
        if (errorCount == 0) {
            cout << "No errors found." << endl;
        } else {
            cout << "Error tokens:" << endl;
            for (int j = 0; j < errorCount; ++j) {
                cout << errors[j] << endl;
            }
        }
    }
    
    bool exportToFiles(const string& tokenFile = "Token.txt", const string& errorFile = "Error.txt") {
        ofstream fout1(tokenFile.c_str());
        if (!fout1) return false;
        for (int j = 0; j < tokenCount; ++j) {
            fout1 << tokens[j].lexeme << " -> " << tokens[j].type << endl;
        }
        fout1.close();
        ofstream fout2(errorFile.c_str());
        if (!fout2) return false;
        for (int j = 0; j < errorCount; ++j) {
            fout2 << errors[j] << endl;
        }
        fout2.close();
        return true;
    }
};

int main() {
    LexicalAnalyzer lexer;
    string choice;
    bool running = true;
    while (running) {
        cout << "\n====== Lexical Analyzer Menu ======" << endl;
        cout << "1. Load Source Code" << endl;
        cout << "2. Analyze Code and Detect Tokens" << endl;
        cout << "3. View Token Types Separately" << endl;
        cout << "4. Show Errors" << endl;
        cout << "5. Export Token and Error Files" << endl;
        cout << "6. Exit" << endl;
        cout << "Enter your choice: ";
        if (!getline(cin, choice)) break;
        if (choice.empty()) continue;
        
        switch (choice[0]) {
            case '1': {
                cout << "Load Source Code:\n";
                cout << "1. Enter code manually\n";
                cout << "2. Load from file\n";
                cout << "3. Cancel\n";
                cout << "Select option: ";
                string subChoice;
                if (!getline(cin, subChoice) || subChoice.empty()) break;
                if (subChoice[0] == '1') {
                    // manual input
                    cout << "Enter your source code (end with a single line containing `END`):" << endl;
                    string inputCode;
                    string line;
                    while (true) {
                        if (!getline(cin, line)) break;
                        if (line == "END") break;
                        inputCode += line;
                        inputCode.push_back('\n');
                    }
                    lexer.setSourceCode(inputCode);
                    cout << "Source code loaded from user input." << endl;
                } else if (subChoice[0] == '2') {
                    cout << "Enter filename: ";
                    string filename;
                    if (!getline(cin, filename)) filename.clear();
                    if (!filename.empty() && lexer.loadFromFile(filename)) {
                        cout << "Source code loaded from file \"" << filename << "\"." << endl;
                    } else {
                        cout << "Failed to open file: " << filename << endl;
                    }
                } else {
                    cout << "Canceled loading source code." << endl;
                }
                break;
            }
            case '2': {
                lexer.analyze();
                cout << "Lexical analysis complete. Tokens identified:" << endl;
                lexer.printAllTokens();
                break;
            }
            case '3': {
                cout << "Select token category to view:\n";
                cout << "1. Identifiers\n";
                cout << "2. Numbers\n";
                cout << "3. Operators\n";
                cout << "4. Punctuations\n";
                cout << "5. Keywords\n";
                cout << "6. Back\n";
                string typeOpt;
                if (!getline(cin, typeOpt) || typeOpt.empty()) break;
                int opt = typeOpt[0] - '0';
                if (opt >= 1 && opt <= 5) {
                    lexer.viewTokensByType(opt);
                }
                // if 6 or invalid, just go back to main menu
                break;
            }
            case '4': {
                lexer.printErrors();
                break;
            }
            case '5': {
                if (lexer.exportToFiles()) {
                    cout << "Tokens and errors have been exported to Token.txt and Error.txt." << endl;
                } else {
                    cout << "Failed to write to files." << endl;
                }
                break;
            }
            case '6': {
                cout << "Exiting program. Goodbye." << endl;
                running = false;
                break;
            }
            default:
                cout << "Invalid choice, please try again." << endl;
        }
    }
    return 0;
}
