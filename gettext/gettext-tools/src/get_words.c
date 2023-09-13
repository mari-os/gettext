#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <message.h>
#include <read-mo.h>
#include <libpq-fe.h>
#include <stdlib.h>

/* static void insert(const char const* eng, const char const* rus) {
    printf ("%s\t%s\n", eng, rus);
} */

static void
exit_nicely(PGconn *conn)
{
    PQfinish(conn);
    exit(1);
}

static PGconn *initPGconn()
{
    PGconn *conn = PQconnectdb("host=192.168.2.156 dbname = postgres");
    if (PQstatus(conn) != CONNECTION_OK)
    {
        fprintf(stderr, "Connection to database failed: %s",
                PQerrorMessage(conn));
        exit_nicely(conn);
    }

    PGresult *resPr = PQprepare(conn, "ins1", "insert into t(file, eng, rus) values ($1, $2, $3)", 3, NULL);
    if (PQresultStatus(resPr) != PGRES_COMMAND_OK)
    {
        fprintf(stderr, "command failed: %s", PQerrorMessage(conn));
        PQclear(resPr);
        exit_nicely(conn);
    }
    PQclear(resPr);

    return conn;
}

static void insert(PGconn *conn, const char *file, const char *eng, const char *rus)
{
    const char* paramValues[] = {file, eng, rus};
    PGresult *resIns = PQexecPrepared(conn,
           "ins1",
           3,
           paramValues,
           NULL,
           NULL,
           0);
    if (PQresultStatus(resIns) != PGRES_COMMAND_OK)
    {
        fprintf(stderr, "command failed: %s", PQerrorMessage(conn));
        PQclear(resIns);
        exit_nicely(conn);
    }
    PQclear(resIns);
}

int main (int argc, char **argv) {
    if (argc != 1 + 1) {
        fprintf(stderr, "Usage: %s filename.mo\n", argv[0]);
        return 1;
    }

    const char* filename = argv[1];

    message_list_ty *mlp = message_list_alloc (false);
    read_mo_file (mlp, filename);

    PGconn *conn = initPGconn();
    for (int i = 0; i < mlp->nitems; i++) {
	insert(conn, filename, mlp->item[i]->msgid, mlp->item[i]->msgstr);
    }
    PQfinish(conn);

    return 0;
}

/*
create table t (file text, eng text, rus text);


cd /home/keremet/compile/gettext/gettext/gettext-tools/src
gcc -DLOCALEDIR=\"/usr/local/share/locale\" -DBISON_LOCALEDIR=\"/usr/share/locale\" -DLOCALE_ALIAS_PATH=\"/usr/local/share/locale\" -DUSEJAVA=0 -DGETTEXTJAR=\"/usr/local/share/gettext/gettext.jar\" -DLIBDIR=\"/usr/local/lib\" -DGETTEXTDATADIR=\"/usr/local/share/gettext\" -DPROJECTSDIR=\"/usr/local/share/gettext/projects\" -DEXEEXT=\"\" -DHAVE_CONFIG_H -I. -I..  -I. -I. -I.. -I.. -I../libgrep -I../gnulib-lib -I../gnulib-lib -I../intl -I../../gettext-runtime/intl -DINSTALLDIR=\"/usr/local/bin\"  -I/home/keremet/compile/postgresql_bin/include/  -g -O2 -c -o get_words.o get_words.c
gcc -g -O2 get_words.o msgunfmt-read-mo.o ../../gettext-runtime/intl/msgunfmt-hash-string.o  ./.libs/libgettextsrc.so /home/keremet/compile/gettext/gettext/gettext-tools/gnulib-lib/.libs/libgettextlib.so -lxml2 /home/keremet/compile/gettext/gettext/libtextstyle/lib/.libs/libtextstyle.so -lm -lncurses -lc -Wl,-rpath -Wl,/usr/local/lib /home/keremet/compile/postgresql_bin/lib/libpq.a 
 */
