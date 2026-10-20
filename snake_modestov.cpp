// snake_modestov.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <fstream>
#include <conio.h>
#include <windows.h>
using namespace std;

const int W = 30;
const int H = 15;

struct Segment {
    int x;
    int y;
    Segment* next;
};

struct Game {
    Segment* head = nullptr;
    int dx = 1;
    int dy = 0;
    int foodX = 0;
    int foodY = 0;
    int score = 0;
    int length = 0;
};

struct Record {
    char name[50];
    int score;
};

void clearSnake(Game& game) {
    while (game.head != nullptr) {
        Segment* temp = game.head;
        game.head = game.head->next;
        delete temp;
    }
}

void placeFood(Game& game) {
    bool onSnake = true;
    while (onSnake) {
        game.foodX = rand() % (W - 2) + 1;
        game.foodY = rand() % (H - 2) + 1;
        onSnake = false;
        Segment* cur = game.head;
        while (cur != nullptr) {
            if (cur->x == game.foodX && cur->y == game.foodY) {
                onSnake = true;
            }
            cur = cur->next;
        }
    }
}

void newGame(Game& game) {
    clearSnake(game);
    game.dx = 1;
    game.dy = 0;
    game.score = 0;
    game.length = 3;
    Segment* last = nullptr;
    for (int i = 0; i < 3; i++) {
        Segment* s = new Segment;
        s->x = W / 2 - i;
        s->y = H / 2;
        s->next = nullptr;
        if (last == nullptr) {
            game.head = s;
        }
        else {
            last->next = s;
        }
        last = s;
    }
    placeFood(game);
}

void draw(Game& game) {
    system("cls");
    for (int y = 0; y < H; y++) {
        for (int x = 0; x < W; x++) {
            if (y == 0 || y == H - 1 || x == 0 || x == W - 1) {
                cout << '#';
            }
            else if (x == game.head->x && y == game.head->y) {
                cout << 'O';
            }
            else {
                char c = ' ';
                if (x == game.foodX && y == game.foodY) {
                    c = '*';
                }
                Segment* cur = game.head->next;
                while (cur != nullptr) {
                    if (cur->x == x && cur->y == y) {
                        c = 'o';
                    }
                    cur = cur->next;
                }
                cout << c;
            }
        }
        cout << endl;
    }
    cout << "Счёт: " << game.score << "  Длина: " << game.length << endl;
    cout << "W A S D - движение, P - сохранить, Q - в меню" << endl;
}

void logField(Game& game, ofstream& log) {
    for (int y = 0; y < H; y++) {
        for (int x = 0; x < W; x++) {
            if (y == 0 || y == H - 1 || x == 0 || x == W - 1) {
                log << '#';
            }
            else if (x == game.head->x && y == game.head->y) {
                log << 'O';
            }
            else {
                char c = ' ';
                if (x == game.foodX && y == game.foodY) {
                    c = '*';
                }
                Segment* cur = game.head->next;
                while (cur != nullptr) {
                    if (cur->x == x && cur->y == y) {
                        c = 'o';
                    }
                    cur = cur->next;
                }
                log << c;
            }
        }
        log << endl;
    }
}

void saveGame(Game& game) {
    ofstream out("save.dat", ios::binary);
    out.write((char*)&game.score, sizeof(int));
    out.write((char*)&game.dx, sizeof(int));
    out.write((char*)&game.dy, sizeof(int));
    out.write((char*)&game.foodX, sizeof(int));
    out.write((char*)&game.foodY, sizeof(int));
    out.write((char*)&game.length, sizeof(int));
    Segment* cur = game.head;
    while (cur != nullptr) {
        out.write((char*)&cur->x, sizeof(int));
        out.write((char*)&cur->y, sizeof(int));
        cur = cur->next;
    }
    out.close();
}

bool loadGame(Game& game) {
    ifstream in("save.dat", ios::binary);
    if (!in.is_open()) {
        return false;
    }
    clearSnake(game);
    in.read((char*)&game.score, sizeof(int));
    in.read((char*)&game.dx, sizeof(int));
    in.read((char*)&game.dy, sizeof(int));
    in.read((char*)&game.foodX, sizeof(int));
    in.read((char*)&game.foodY, sizeof(int));
    in.read((char*)&game.length, sizeof(int));
    Segment* last = nullptr;
    for (int i = 0; i < game.length; i++) {
        Segment* s = new Segment;
        in.read((char*)&s->x, sizeof(int));
        in.read((char*)&s->y, sizeof(int));
        s->next = nullptr;
        if (last == nullptr) {
            game.head = s;
        }
        else {
            last->next = s;
        }
        last = s;
    }
    in.close();
    return true;
}

int play(Game& game) {
    ofstream log("log.txt", ios::app);
    bool turned = false;
    log << "--- новая игра ---" << endl;
    while (true) {
        draw(game);
        Sleep(1000);

        if (_kbhit()) {
            char key = _getch();
            int oldDx = game.dx;
            int oldDy = game.dy;
            if (key >= 'A' && key <= 'Z') {
                key = key + 32;
            }
            if (key == 'w' && game.dy != 1) {
                game.dx = 0;
                game.dy = -1;
            }
            else if (key == 's' && game.dy != -1) {
                game.dx = 0;
                game.dy = 1;
            }
            else if (key == 'a' && game.dx != 1) {
                game.dx = -1;
                game.dy = 0;
            }
            else if (key == 'd' && game.dx != -1) {
                game.dx = 1;
                game.dy = 0;
            }
            else if (key == 'p') {
                saveGame(game);
                cout << "Игра сохранена" << endl;
                Sleep(700);
            }
            else if (key == 'q') {
                return 0;
            }
            if (game.dx != oldDx || game.dy != oldDy) {
                log << "поворот '" << key << "' в точке (" << game.head->x << "," << game.head->y << ")" << endl;
                turned = true;
            }
        }

        int nx = game.head->x + game.dx;
        int ny = game.head->y + game.dy;

        if (nx <= 0 || nx >= W - 1 || ny <= 0 || ny >= H - 1) {
            log << "КОНЕЦ: врезалась в стену в точке (" << nx << "," << ny << "), счёт " << game.score << endl;
            logField(game, log);
            return 1;
        }

        Segment* cur = game.head;
        while (cur != nullptr) {
            if (cur->x == nx && cur->y == ny) {
                log << "КОНЕЦ: врезалась в себя в точке (" << nx << "," << ny << "), счёт " << game.score << endl;
                logField(game, log);
                return 1;
            }
            cur = cur->next;
        }

        Segment* temp = new Segment;
        temp->x = nx;
        temp->y = ny;
        temp->next = game.head;
        game.head = temp;

        if (nx == game.foodX && ny == game.foodY) {
            game.score = game.score + 10;
            game.length++;
            log << "съела еду в точке (" << nx << "," << ny << "), счёт " << game.score << endl;
            placeFood(game);
        }
        else {
            Segment* prev = game.head;
            while (prev->next->next != nullptr) {
                prev = prev->next;
            }
            delete prev->next;
            prev->next = nullptr;
        }

        if (turned) {
            logField(game, log);
            turned = false;
        }
    }
}

void saveRecord(Game& game) {
    Record r;
    cout << "Игра окончена! Ваш счёт: " << game.score << endl;
    cout << "Введите имя: ";
    cin >> r.name;
    r.score = game.score;
    ofstream out("records.dat", ios::binary | ios::app);
    out.write((char*)&r, sizeof(Record));
    out.close();
}

void showRecords() {
    ifstream in("records.dat", ios::binary);
    if (!in.is_open()) {
        cout << "Рекордов пока нет" << endl;
        return;
    }
    Record list[100];
    int count = 0;
    while (count < 100 && in.read((char*)&list[count], sizeof(Record))) {
        count++;
    }
    in.close();

    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - 1 - i; j++) {
            if (list[j].score < list[j + 1].score) {
                Record temp = list[j];
                list[j] = list[j + 1];
                list[j + 1] = temp;
            }
        }
    }

    cout << "--- Таблица рекордов ---" << endl;
    for (int i = 0; i < count && i < 10; i++) {
        cout << i + 1 << ". " << list[i].name << " - " << list[i].score << endl;
    }
}

int main() {
    setlocale(LC_ALL, "Russian");
    Game game;
    int choice = 0;

    while (choice != 4) {
        cout << endl;
        cout << "1. Новая игра" << endl;
        cout << "2. Загрузить игру" << endl;
        cout << "3. Таблица рекордов" << endl;
        cout << "4. Выход" << endl;
        cout << "Выбор: ";
        cin >> choice;

        switch (choice) {
        case 1:
            newGame(game);
            if (play(game) == 1) {
                saveRecord(game);
            }
            clearSnake(game);
            break;
        case 2:
            if (loadGame(game)) {
                if (play(game) == 1) {
                    saveRecord(game);
                }
                clearSnake(game);
            }
            else {
                cout << "Сохранение не найдено" << endl;
            }
            break;
        case 3:
            showRecords();
            break;
        case 4:
            break;
        default:
            cout << "Нет такого пункта" << endl;
        }
    }

    clearSnake(game);
    return 0;
}

// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
