#include "client.h"

#include <stdio.h>

void print_title() {
    printf("\033[1;34m");  // 파란색 테두리
    printf(" █████████████████████████████████████████████\n");
    printf(" █                                           █\n");
    printf(" █            \033[1;31m EEMBEDDED SYSTEM\033[1;34m              █\n");
    printf(" █                                           █\n");
    printf(" █████████████████████████████████████████████\n");
    printf("\033[0m\n");
}

void print_subtitle() {
    printf("\033[1;36m");
    printf("  ██████████████████████████████████████████\n");
    printf("  █             Term Project               █\n");
    printf("  █           \033[1;33m박준형 & 이정근\033[1;36m              █\n");
    printf("  ██████████████████████████████████████████\n");
    printf("\033[0m\n");
}

void exit_banner() {
	printf("\033[1;38m");
    printf("  ██████████████████████████████████████████\n");
    printf("  █                  EXIT                  █\n");
    printf("  █        								   █\n");
    printf("  ██████████████████████████████████████████\n");
    printf("\033[0m\n");
}