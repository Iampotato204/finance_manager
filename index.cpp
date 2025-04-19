#include <iostream>
#include <cstdlib>
#include <cstring>
#include <stdio.h>
#include <algorithm>
#include <vector>
#include <string>

#include <cgicc/CgiDefs.h>
#include <cgicc/Cgicc.h>
#include <cgicc/FormEntry.h>
#include <cgicc/CgiEnvironment.h>
#include <cgicc/HTTPHTMLHeader.h>
#include <cgicc/HTMLClasses.h>

#include <sqlite3.h>

#define html_begin "<!DOCTYPE html><head><meta charset=\"UTF-8\"><title>Finance manager</title></head>"
#define css "<style>.table1{border:3px solid red}.books_parent{border:5px solid #ff0;display:flex;flex-wrap:wrap;align-items:flex-start;align-items:center}.book_child{border:3px solid #ff0;max-width:500px}.table2{border:1px solid red;margin:0}th{border:3px solid green;max-width:300px;border-radius: 8px;}td{border:1px solid green;min-height:10px}thead,tr{border:1px solid #00f;min-height:10px}.but{width:70px}.fon{width:70px;text-align:center}.cu{background-color:#c6ff9d}.cn{background-color:#fff99d}.cd{background-color:#ff9d9d}.cb{background-color:#fff}.cu_d{background-color:#95bf76}.cn_d{background-color:#bfbb76}.cd_d{background-color:#bf7676}.cb_d{background-color:#bfbfbf}.inum{background-color:#fff99d;width:60px}.inote{background-color:#fff99d;width:97%;}.di_inote{background-color:#bfbb76;width:100%;text-align:left}.ihead{background-color:#fff99d}</style>"

#define body_begin "<body><div class=books_parent>"
#define table_begin_template "<div class=book_child><form action='' method='POST'><table class=table2><thead><th></th><th>%s</th><th></th></thead><tr><td class=cn><input type=input name=valnu%05d00000 class=inum placeholder='+-00,00' /></td><td><input type=input name=valna%05d00000 class=inote placeholder='Note to self' /></td><td class=cu><input type=submit name=butnf%05d00000 class='but cu' value=Add /></td></tr>"
#define row_template "<tr><td class=c%c>%2.2f</td><td>%s</td><td class=cd><input type=submit name=butrf%05d%05d class='but cd' value=Remove /></td></tr>"
#define table_end "</table></form></div>"

#define table_di "<div class=book_child ><form action='' method='POST'><table class=table2 ><thead><th></th><th><input type=input name=valtn%05d00000 class=inote placeholder='New transaction book' /></th><th><input type=submit name=butnt0000000000 class='but cu' value=Create ></th></thead><tr><td class=cn_d>+-00.00</td><td class=cb><div class=di_inote>Note to self</div></td><td class=cu_d><div class='fon cu_d'>Add</div></td></tr><tr><td class=cu_d>+1.00</td><td class=cb_d>Note 2</td><td class=dtd><div class='fon cd_d'>Remove</div></td></tr><tr><td class=cd_d>-1.00</td><td class=cb_d>Note 1</td><td class=dtd><div class='fon cd_d'>Remove</div></td></tr></table></form></div>"
#define body_end "</div></body>"

using namespace std;
using namespace cgicc;

const char* ENV[ 24 ] = {
    "COMSPEC",
    "DOCUMENT_ROOT",
    "GATEWAY_INTERFACE",
    "HTTP_ACCEPT",
    "HTTP_ACCEPT_ENCODING",
    "HTTP_ACCEPT_LANGUAGE",
    "HTTP_CONNECTION",
    "HTTP_HOST",
    "HTTP_USER_AGENT",
    "PATH",
    "QUERY_STRING",
    "REMOTE_ADDR",
    "REMOTE_PORT",
    "REQUEST_METHOD",
    "REQUEST_URI",
    "SCRIPT_FILENAME",
    "SCRIPT_NAME",
    "SERVER_ADDR",
    "SERVER_ADMIN",
    "SERVER_NAME",
    "SERVER_PORT",
    "SERVER_PROTOCOL",
    "SERVER_SIGNATURE",
    "SERVER_SOFTWARE"
};

using namespace cgicc;

vector<int> book_ids={};
struct s_name{
    char text[100];
};
vector<s_name> book_names={};

//char temp_row[500];
size_t strlen(const char *str){
    const char *s;
    for (s = str; *s; ++s);
    return(s - str);
}

static int book_callback(void *NotUsed, int argc, char **argv, char **azColName) {
    book_ids.push_back(atoi(argv[0]));
    s_name tab_label;
    strcpy(tab_label.text,argv[1]);
    book_names.push_back(tab_label);
    return 0;
}

static int trans_callback(void *NotUsed, int argc, char **argv, char **azColName) {
    // argv's : id,table_id,val,note
    float temp_val = atof(argv[2]);
    printf (row_template,
            (temp_val<0 ?'d':'u'),
            temp_val,
            argv[3], // temp_lable_name.c_str()
            atoi(argv[1]), // temp_table_number
            atoi(argv[0])); // temp_field_number)
//    for(int i = 0; i < argc; i++) {
///        printf("%s = %s<br>\n", azColName[i], is_null_pointer(argv[i]) ? "NULL" : argv[i]); //        printf("%s\n", !(argv[i]) ? "NULL" : argv[i]);
//    }
    return 0;
}

// sudo g++ index.cpp;./a.out
int main(){
    try {
        Cgicc cgi;

        // Send HTTP header
        // cout << HTTPHTMLHeader() << endl; //+ documented way
        // printf("%s\n\n",HTTPHTMLHeader().getData().cstr()); //- getData() result differs from cout`ing object
        printf("Content-Type: text/html\n\n"); //+ not the preferred solution, may cause difficulties with headers but i did not want to use cout

        // printf ("%.21s\nhi\n%.7s\n",html_begin,html_end);
        printf ("%.80s\n",html_begin);
        printf ("%.855s\n",css);
//        printf("ENVIRONMENT VARS:<br>\n");
//        for (int i=0; i<24; i++){
//            printf("%s : ",ENV[i]);
//            char *value = std::getenv(ENV[i]);
//            if (value != 0){printf("%s<br>\n",value);}
//            else {printf("\r");} // printf("Environment variable does not exist.\n");
//        }
        printf("<br>\n");

        sqlite3 *db;
        sqlite3_stmt *statement = 0;
        char *err_msg = 0;

        int rc = sqlite3_open_v2("file:data.db", &db, SQLITE_OPEN_READWRITE,nullptr);
        if (rc != SQLITE_OK) {
            cout<< stderr<< " Cannot open database: " << sqlite3_errmsg(db) <<endl;
            sqlite3_close(db);
        }

        auto elements = cgi.getElements();
//        auto post = cgi.getEnvironment().getPostData();
//        cout << post << endl;
//        printf("CGI VARS:<br>\n");
        // std::for_each(elements.cbegin(),elements.cend(), [](cgicc::FormEntry &fe) {printf("%s \n",fe.getName());});

        s_name new_note;
        s_name new_table;
        float new_val=0;
//        bool new_row=false;
//        bool delete_row=false;

        for(int i=0; i<elements.size(); i++){
//            cout<<elements.at(i).getName()<<endl;
            cgicc::FormEntry elem = elements.at(i);
            char elemName[16];
            strcpy(elemName, elem.getName().c_str());
            char tag[6];
            memset(tag, 0,6);
            memcpy(tag, elemName,5);
            char tablestr[6];
            memset(tablestr, 0,6);
            memcpy(tablestr, elemName+5,5);
            char fieldstr[6];
            memset(fieldstr, 0,6);
            memcpy(fieldstr, elemName+10,5);
//            printf("tag:%s, table:%s, field:%s, value:%s \n<br>", tag, tablestr, fieldstr, elem.getValue().c_str());
//            int tablenum = elemName.atoi();
//            printf("tablenum:%d<br>",tablenum);
            char tmpsql[300];
            memset(tmpsql,0,300);

            if (strcmp(tag,"valna")==0){
                strcpy(new_note.text, elem.getValue().c_str());
//                printf("note name processed <br>");
            }else if (strcmp(tag,"valnu")==0){
                new_val=atof(elem.getValue().c_str());
//                printf("note money processed <br>");
            }else if (strcmp(tag,"valtn")==0){
                strcpy(new_table.text, elem.getValue().c_str());
            }else if(strcmp(tag,"butnf")==0){
//                new_row=true;
                sprintf(tmpsql,"insert into transactions(book, `transaction`, name) values('%d','%.2f','%s');", atoi(tablestr), new_val, new_note.text);
                rc = sqlite3_exec(db, tmpsql, 0, 0, &err_msg);
//                printf("new row requested.<br>");
//                printf("db responce: {");cout <<rc;printf("}<br>");
            }else if (strcmp(tag,"butrf")==0){
//                delete_row=true;
                sprintf(tmpsql,"delete from transactions where id=%d;", atoi(fieldstr), new_val, new_note.text);
//                printf("%s<br>",tmpsql);
                rc = sqlite3_exec(db, tmpsql, 0, 0, &err_msg);
//                printf("delete row requested.<br>");
//                printf("db responce: {");cout <<rc;printf("}<br>");
            }else if (strcmp(tag,"butnt")==0){
//                printf("nqme len: %d<br>",strlen(new_table.text));
                if (strlen(new_table.text)<1){
                    strcpy(new_table.text, "New transaction book");
                }
                sprintf(tmpsql,"insert into book(name) values('%s');", new_table.text);
                rc = sqlite3_exec(db, tmpsql, 0, 0, &err_msg);
//                printf("new table requested <br>");
//                printf("db responce: {");cout <<rc;printf("}<br>");
            }else{
                printf("unknown element %s<br>",elemName);
            }
        }

        char sql[] = "select * from book;";
//        rc = sqlite3_prepare_v2(db, sql, -1, &statement, 0);
        rc = sqlite3_exec(db, sql, book_callback, 0, &err_msg);
        char smolsql[70];

        printf (body_begin);
        for(int i=0; i<book_ids.size(); i++){
            printf (table_begin_template,
                book_names[i].text,
                book_ids.at(i),
                book_ids.at(i),
                book_ids.at(i));
//            printf("%d<br>",book_ids.at(tablenum));
            sprintf(smolsql, "select * from transactions where book='%d' order by id desc;", book_ids.at(i));
//            printf("%s<br>",smolsql);
            rc = sqlite3_exec(db, smolsql, trans_callback, 0, &err_msg);
            printf(table_end);
        }

        printf (table_di);

        printf (body_end);

    }
    catch(exception& e) {printf("error in cgi");}

    return 0;
}
