#include <iostream>
#include "Bot.h"

using namespace std;

int main() {
    string triggers[100];
    string responses[100];
    int count = 0;

    loadResponses(triggers, responses, count);
    startBot(triggers, responses, count);

    return 0;
}