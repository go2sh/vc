#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <Lexer/Lexer.h>

using namespace vc;

int main(int argc, char ** argv) {
    int fd = open("tests/test.vhd", O_RDONLY);
    struct stat st;
    void *data;

    fstat(fd,&st);
    data = mmap(NULL,st.st_size, PROT_READ,MAP_PRIVATE,fd, 0);
    
    Lexer lexer((char*)data, st.st_size);
    Token t;
    lexer.Lex(t);
    
    printf("Token: %d", t.getTokenKind());
    close(fd);
    return 0;
}