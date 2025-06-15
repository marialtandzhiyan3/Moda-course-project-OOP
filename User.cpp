#include "User.h"
#include <fstream>

User::User(const CharArray& name, const CharArray& egn, const CharArray& password)
    : name(name), egn(egn), password(password) {}


bool User::authenticate(const CharArray& inputPassword) const {
    return password == inputPassword;
}

const CharArray& User::getName() const {
    return name;
}

const CharArray& User::getEgn() const {
    return egn;
}

void User::saveToFile(std::ofstream& out) const {
    out << name.getData() << std::endl;
    out << egn.getData() << std::endl;
    out << password.getData() << std::endl;
}
void User::loadFromFile(std::ifstream& in) {
    char buffer[256];

    in.getline(buffer, sizeof(buffer));
    name = CharArray(buffer);

    in.getline(buffer, sizeof(buffer));
    egn = CharArray(buffer);

    in.getline(buffer, sizeof(buffer));
    password = CharArray(buffer);
}
