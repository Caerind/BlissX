#include "VirtualMachine.hpp"

#include <cstdio>
#include <cstdlib>
#include <string>

void repl(VirtualMachine& virtualMachine)
{
    char line[1024];
    for(;;)
    {
        printf("> ");
        if (!fgets(line, sizeof(line), stdin))
        {
            printf("\n");
            break;
        }
        virtualMachine.interpret(line);
    }
}

char* readFile(const char* path)
{
    FILE* file = fopen(path, "rb");
    if (file == NULL)
    {
        fprintf(stderr, "Could not open file \"%s\".\n", path);
        exit(74);
    }

    fseek(file, 0L, SEEK_END);
    std::size_t fileSize = ftell(file);
    rewind(file);

    char* buffer = (char*)malloc(fileSize + 1);
    if (buffer == NULL)
    {
        fprintf(stderr, "Not enought memory to read \"%s\".\n", path);
        exit(74);
    }

    std::size_t bytesRead = fread(buffer, sizeof(char), fileSize, file);
    if (bytesRead < fileSize)
    {
        fprintf(stderr, "Could not read file \"%s\".\n", path);
        exit(74);
    }

    buffer[bytesRead] = '\0';

    fclose(file);
    return buffer;
}

void runFile(VirtualMachine& virtualMachine, const char* path)
{
    char* source = readFile(path);
    VirtualMachine::InterpretResult result = virtualMachine.interpret(source);
    free(source);

    if (result == VirtualMachine::Interpret_CompileError) exit(65);
    if (result == VirtualMachine::Interpret_RuntimeError) exit(70);
}

int main(int argc, char** argv)
{
    if (argc == 1)
    {
        VirtualMachine virtualMachine;
        repl(virtualMachine);
    }
    else if (argc == 2)
    {
        VirtualMachine virtualMachine;
        runFile(virtualMachine, argv[1]);
    }
    else
    {
        fprintf(stderr, "Usage: lox [path]\n");
        exit(64);
    }
	return 0;
}
