#ifndef DEFINITIONS_H
#define DEFINITIONS_H


#define DISCORD dpp::cluster

#define UPOINTER std::unique_ptr
#define SPOINTER std::shared_ptr

#define CREATE_UPOINTER(type, ...) std::make_unique<type>(__VA_ARGS__)

#define VAR auto

#define VAL const auto


#endif // DEFINITIONS_H
