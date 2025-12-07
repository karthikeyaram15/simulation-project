

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ACCTS 100
#define NAME_LEN  32
#define LOGFILE   "bank_log.txt"

typedef struct {
    int id;
    char name[NAME_LEN];
    double bal;
    int used;
} Account;

Account accts[MAX_ACCTS];
int nextId = 1;

void log_msg(const char *s){
    FILE *f = fopen(LOGFILE, "a");
    if(!f) return;
    fprintf(f, "%s\n", s);
    fclose(f);
}

Account* find_acc(int id){
    for(int i=0;i<MAX_ACCTS;i++) if(accts[i].used && accts[i].id==id) return &accts[i];
    return NULL;
}

int create_account(const char *name, double init){
    for(int i=0;i<MAX_ACCTS;i++){
        if(!accts[i].used){
            accts[i].used = 1;
            accts[i].id = nextId++;
            strncpy(accts[i].name, name, NAME_LEN-1);
            accts[i].name[NAME_LEN-1]=0;
            accts[i].bal = init;
            char buf[128]; snprintf(buf,sizeof(buf),"CREATE %d %s %.2f", accts[i].id, accts[i].name, accts[i].bal);
            log_msg(buf);
            return accts[i].id;
        }
    }
    return -1;
}

int deposit(int id, double amt){
    Account *a = find_acc(id);
    if(!a || amt<=0) return 0;
    a->bal += amt;
    char buf[128]; snprintf(buf,sizeof(buf),"DEPOSIT %d %.2f -> %.2f", id, amt, a->bal);
    log_msg(buf);
    return 1;
}

int withdraw(int id, double amt){
    Account *a = find_acc(id);
    if(!a || amt<=0 || a->bal < amt) return 0;
    a->bal -= amt;
    char buf[128]; snprintf(buf,sizeof(buf),"WITHDRAW %d %.2f -> %.2f", id, amt, a->bal);
    log_msg(buf);
    return 1;
}

int transfer(int from, int to, double amt){
    Account *A = find_acc(from), *B = find_acc(to);
    if(!A || !B || amt<=0 || A->bal < amt) return 0;
    A->bal -= amt;
    B->bal += amt;
    char buf[192]; snprintf(buf,sizeof(buf),"TRANSFER %d->%d %.2f | %d:%.2f %d:%.2f",
                             from,to,amt, from,A->bal, to,B->bal);
    log_msg(buf);
    return 1;
}

void show_all(){
    printf("ID | Name             | Balance\n");
    printf("---+------------------+---------\n");
    for(int i=0;i<MAX_ACCTS;i++) if(accts[i].used)
        printf("%2d | %-16s | %8.2f\n", accts[i].id, accts[i].name, accts[i].bal);
}

int main(){
    /* initialize log */
    FILE *f = fopen(LOGFILE,"w"); if(f){ fprintf(f,"Bank log start\n"); fclose(f); }

    int ch;
    while(1){
        printf("\n1.Create 2.Deposit 3.Withdraw 4.Transfer 5.Show 6.Exit\nChoice: ");
        if(scanf("%d",&ch)!=1) break;
        if(ch==1){
            char name[NAME_LEN]; double init;
            printf("Name: "); scanf("%s",name);
            printf("Initial deposit: "); scanf("%lf",&init);
            int id = create_account(name, init);
            if(id>0) printf("Account created. ID=%d\n", id);
            else printf("Create failed.\n");
        } else if(ch==2){
            int id; double amt; printf("ID: "); scanf("%d",&id);
            printf("Amount: "); scanf("%lf",&amt);
            printf(deposit(id,amt) ? "Deposited.\n" : "Deposit failed.\n");
        } else if(ch==3){
            int id; double amt; printf("ID: "); scanf("%d",&id);
            printf("Amount: "); scanf("%lf",&amt);
            printf(withdraw(id,amt) ? "Withdrawn.\n" : "Withdraw failed.\n");
        } else if(ch==4){
            int a,b; double amt; printf("From ID: "); scanf("%d",&a);
            printf("To ID: "); scanf("%d",&b); printf("Amount: "); scanf("%lf",&amt);
            printf(transfer(a,b,amt) ? "Transferred.\n" : "Transfer failed.\n");
        } else if(ch==5){
            show_all();
        } else break;
    }
    printf("Goodbye.\n");
    return 0;
}



