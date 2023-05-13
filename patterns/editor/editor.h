#pragma once
#include <iostream>
#include <memory>
#include <vector>
#include <string>

struct Command {
    Command(){};
    virtual void Do() = 0;
    virtual void Undo() = 0;
    virtual ~Command() = default;
};

struct BackspaceCommand : Command {
    BackspaceCommand(size_t& c, std::string* s) {
        str = s;
        curs = &c;
        k = *s;
    }

    void Do() override {
        if (*curs != 0) {
            --*curs;
            std::string new_str;
            bool flag = false;
            size_t j = 0;
            while (j != str->size()) {
                if (*curs == j && !flag) {
                    flag = true;
                    ++j;
                } else {
                    new_str += (*str)[j];
                    ++j;
                }
            }
            *str = new_str;
        }
    }
    void Undo() override {
        //        std::string new_str;
        //        bool flag = false;
        //        size_t j = 0;
        //        while (j != str->size()) {
        //            if (*curs == j && !flag) {
        //                new_str += k;
        //                flag = true;
        //            } else {
        //                new_str += (*str)[j];
        //                ++j;
        //            }
        //        }
        //        if (new_str.size() == str->size()) {
        //            new_str += k;
        //        }
        //        *str = new_str;
        *str = k;
        ++*curs;
    }

    size_t* curs;
    std::string* str;
    std::string k;
};

struct ShiftLeftCommand : public Command {
    void Do() override {
        if (*curs > 0) {
            --*curs;
        }
    }
    void Undo() override {
        ++*curs;
    }
    ShiftLeftCommand(size_t& c, std::string* s) {
        curs = &c;
        str = s;
    }
    size_t* curs;
    std::string* str;
};

struct ShiftRightCommand : public Command {
    void Do() override {
        if (*curs < str->size()) {
            ++*curs;
        }
    }
    void Undo() override {
        --*curs;
    }

    ShiftRightCommand(size_t& c, std::string* s) {
        curs = &c;
        str = s;
    }
    size_t* curs;
    std::string* str;
};

struct TypeCommand : public Command {
    TypeCommand(){};
    TypeCommand(std::string* s, char sym, size_t& num) {
        str = s;
        c = sym;
        curs = &num;
    }
    void Do() override {
        if (str->size() == *curs) {
            str->push_back(c);
            ++*curs;
        } else {
            std::string new_str;
            bool flag = false;
            size_t j = 0;
            while (j != str->size()) {
                if (*curs == j && !flag) {
                    new_str.push_back(c);
                    flag = true;
                } else {
                    new_str += (*str)[j];
                    ++j;
                }
            }
            *str = new_str;
            ++*curs;
        }
    }
    void Undo() override {
        std::string new_str;
        bool flag = false;
        size_t j = 0;
        --*curs;
        auto a = str->size();
        while (j != a) {
            if (*curs == j && !flag) {
                flag = true;
                ++j;
            } else {
                new_str += (*str)[j];
                ++j;
            }
        }
        *str = new_str;
    }

    std::string* str;
    char c;
    size_t* curs;
};

class Editor {
public:
    const std::string& GetText() const {
        return s;
    }

    void Type(char c) {
        history_command.emplace_back(std::make_unique<TypeCommand>(TypeCommand(&s, c, curs)));
        history_command[history_command.size() - 1]->Do();
        current = history_command.size() - 1;
        flag = false;
    }

    void ShiftLeft() {
        history_command.emplace_back(std::make_unique<ShiftLeftCommand>(curs, &s));
        history_command.back()->Do();
        current = history_command.size() - 1;
    }

    void ShiftRight() {
        history_command.emplace_back(std::make_unique<ShiftRightCommand>(curs, &s));
        auto a = curs;
        history_command.back()->Do();
        if (curs != a) {
            current = history_command.size() - 1;
        }
    }

    void Backspace() {
        history_command.emplace_back(std::make_unique<BackspaceCommand>(curs, &s));
        history_command.back()->Do();
        current = history_command.size() - 1;
    }

    void Undo() {
        if (!s.empty()) {
            history_command[current]->Undo();
            --current;
            flag = true;
        }
    }

    void Redo() {
        if (!s.empty() && flag) {
            history_command[current + 1]->Do();
            ++current;
        }
    }

    std::string s;
    size_t curs = 0;
    bool flag = false;
    size_t current;
    std::vector<std::unique_ptr<Command>> history_command;
};
