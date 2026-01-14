#include <iostream>
#include <string>
#include <iomanip>

using namespace std;

struct SinhVien {
    string id;
    string name;
    double gpa;
};

struct Node {
    SinhVien s;
    Node *next;
};

typedef Node* node;

node makeNode(SinhVien s) {
    node tmp = new Node();
    tmp->s = s;
    tmp->next = NULL;
    return tmp;
}

bool empty(node a) {
    return a == NULL;
}

int size(node a) {
    int cnt = 0;
    while (a != NULL) {
        cnt++;
        a = a->next;
    }
    return cnt;
}

void insertFirst(node &a, SinhVien s) {
    node tmp = makeNode(s);
    if (a == NULL) {
        a = tmp;
    } else {
        tmp->next = a;
        a = tmp;
    }
}

void insertLast(node &a, SinhVien s) {
    node tmp = makeNode(s);
    if (a == NULL) {
        a = tmp;
    } else {
        node p = a;
        while (p->next != NULL) {
            p = p->next;
        }
        p->next = tmp;
    }
}

void insertMiddle(node &a, SinhVien s, int pos) {
    int n = size(a);
    if (pos <= 0 || pos > n + 1) {
        cout << "Vi tri chen khong hop le!" << endl;
        return;
    }
    if (pos == 1) {
        insertFirst(a, s);
        return;
    }
    if (pos == n + 1) {
        insertLast(a, s);
        return;
    }
    node p = a;
    for (int i = 1; i < pos - 1; i++) {
        p = p->next;
    }
    node tmp = makeNode(s);
    tmp->next = p->next;
    p->next = tmp;
}

void deleteFirst(node &a) {
    if (a == NULL) return;
    a = a->next;
}

void deleteLast(node &a) {
    if (a == NULL) return;
    node p = a;
    node before = NULL;
    while (p->next != NULL) {
        before = p;
        p = p->next;
    }
    if (before == NULL) { 
        a = NULL;
    } else {
        before->next = NULL;
    }
}

void deleteMiddle(node &a, int pos) {
    if (pos <= 0 || pos > size(a)) return;
    if (pos == 1) {
        deleteFirst(a);
        return;
    }
    node p = a;
    node before = NULL;
    for (int i = 1; i < pos; i++) {
        before = p;
        p = p->next;
    }
    if (before != NULL) {
        before->next = p->next;
    }
}

void inthongtin(SinhVien s) {
    cout << "ID: " << s.id << ", Ten: " << s.name << ", GPA: " << fixed << setprecision(2) << s.gpa << endl;
}

void in(node a) {
    cout << "--------------------------------\n";
    cout << "Danh sach sinh vien:\n";
    while (a != NULL) {
        inthongtin(a->s);
        a = a->next;
    }
    cout << "--------------------------------\n";
}

void sapxep(node &a) {
    for (node p = a; p->next != NULL; p = p->next) {
        node min = p;
        for (node q = p->next; q != NULL; q = q->next) {
            if (q->s.gpa < min->s.gpa) {
                min = q;
            }
        }
        SinhVien tmp = min->s;
        min->s = p->s;
        p->s = tmp;
    }
}

void nhapThongTin(SinhVien &s) {
    cout << "Nhap ID: "; cin >> s.id;
    cout << "Nhap Ten: "; cin.ignore(); getline(cin, s.name);
    cout << "Nhap GPA: "; cin >> s.gpa;
}

int main() {
    node head = NULL;
    while (1) {
        cout << "-----------------MENU-----------------\n";
        cout << "1. Chen sinh vien vao dau\n";
        cout << "2. Chen sinh vien vao cuoi\n";
        cout << "3. Chen sinh vien vao giua\n";
        cout << "4. Xoa sinh vien o dau\n";
        cout << "5. Xoa sinh vien o cuoi\n";
        cout << "6. Xoa sinh vien o giua\n";
        cout << "7. Duyet danh sach sinh vien\n";
        cout << "8. Sap xep danh sach theo GPA\n";
        cout << "0. Thoat\n";
        cout << "--------------------------------------\n";
        cout << "Nhap lua chon: ";
        int lc; cin >> lc;
        
        if (lc == 1) {
            SinhVien s; nhapThongTin(s);
            insertFirst(head, s);
        }
        else if (lc == 2) {
            SinhVien s; nhapThongTin(s);
            insertLast(head, s);
        }
        else if (lc == 3) {
            int pos; 
            cout << "Nhap vi tri can chen: "; cin >> pos;
            SinhVien s; nhapThongTin(s);
            insertMiddle(head, s, pos);
        }
        else if (lc == 4) {
            deleteFirst(head);
        }
        else if (lc == 5) {
            deleteLast(head);
        }
        else if (lc == 6) {
            int pos;
            cout << "Nhap vi tri can xoa: "; cin >> pos;
            deleteMiddle(head, pos);
        }
        else if (lc == 7) {
            in(head);
        }
        else if (lc == 8) {
            sapxep(head);
            cout << "Da sap xep danh sach theo GPA!\n";
        }
        else if (lc == 0) {
            break;
        }
    }
    return 0;
}
