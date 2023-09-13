#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <message.h>
#include <read-mo.h>
#include <write-mo.h>

//#include <unistd.h>

#define EQ_STR( s1, s2 ) (strcmp(s1, s2) == 0)

static const struct {
    char* english;
    char* mari;
} MAP[] = {
        {"About", "Программе нерген"},
        {"Cancel", "Кораҥдаш"},
        {"Copy", "Копийым ышташ"},
        {"Clear", "Эрыкташ"},
        {"Close", "Петыраш"},
        {"Close Tab", "Вкладкым петыраш"},
        {"Close Window", "Окнам петыраш"},
        {"Cut", "Пӱчкаш"},
        {"Edit", "Вашталташ"},
        {"File System", "Файл системе"},
        {"Find", "Муаш"},
        {"Find...", "Муаш..."},
        {"Find Next", "Умбакыже кычалаш"},
        {"Find Previous", "Ончычсо кычалаш"},
        {"Find and Replace...", "Муаш да алмашташ..."},
        {"Go to...", "Куснаш..."},
        {"Help", "Полыш"},
        {"Home", "Мӧҥгысӧ каталог"},
        {"Log Out", "Лекташ"},
        {"Network", "Вот"},
        {"New", "У"},
        {"New Window", "У окна"},
        {"Open...", "Почаш..."},
        {"Open Tab", "Вкладкым почаш"},
        {"Open Terminal", "Терминалым почаш"},
        {"Paste", "Шындаш"},
        {"Places", "Вер-влак"},
        {"Preferences", "Келыштарыш-влак"},
        {"Preferences...", "Келыштарыш-влак..."},
        {"Quit", "Лекташ"},
        {"Redo", "Пӧртылташ"},
        {"Restart", "Уэш колташ"},
        {"Save", "Aрален кодаш"},
        {"Save As...", "... семын арален кодаш"},
        {"Save All", "Чыла арален кодаш"},
        {"Select All", "Чыла палемдаш"},
        {"Search", "Кычалмаш"},
        {"Shut Down", "Пашам мучашлымаш"},
        {"Trash", "Тор"},
        {"Undo", "Кораҥдаш"},
        {"View", "Сын"},
};

#define ARRAY_SIZE(arr) ( sizeof(arr) / sizeof(arr[0]) )

int main (int argc, char **argv) {
    if (argc != 1 + 2) {
        fprintf(stderr, "Usage: %s filename_in.mo filename_out.mo\n", argv[0]);
        return 1;
    }

    const char* filename_in = argv[1];
    const char* filename_out = argv[2];

    /* Set default value for global variables.  */
    alignment = DEFAULT_OUTPUT_ALIGNMENT;
    byteswap = 0 ^ ENDIANNESS;
    no_hash_table = false;

    message_list_ty *mlp = message_list_alloc (false);
    //      write(1, "1\n", 2);
    read_mo_file (mlp, filename_in);
    //      write(1, "2\n", 2);
    for (int i = 0; i < mlp->nitems; i++) {
        const char* msgid = mlp->item[i]->msgid;
        int msgid_sz = strlen(msgid) + 1;
        char msgid_norm[msgid_sz];
        int norm_pos = 0;
        for (int j = 0; j < msgid_sz; j++ ) {
            if (msgid[j] != '_')
                msgid_norm[norm_pos++] = msgid[j];
        }

        for (int j = 0; j < ARRAY_SIZE(MAP); j++ ) {
            if (EQ_STR(msgid_norm, MAP[j].english)) {
                mlp->item[i]->msgstr = MAP[j].mari;
                mlp->item[i]->msgstr_len = strlen(mlp->item[i]->msgstr) + 1;
                printf ("%s\t%s\n", msgid, msgid_norm);
                printf ("--------> %s\n", mlp->item[i]->msgstr);
            }
        }
    }

    msgdomain_write_mo (mlp, "123", filename_out);
    //      write(1, "3\n", 2);
    return 0;
}

/*
sudo apt-get install gettext-tools
cd /home/keremet/compile/gettext/gettext/gettext-tools/src
gcc -DLOCALEDIR=\"/usr/local/share/locale\" -DBISON_LOCALEDIR=\"/usr/share/locale\" -DLOCALE_ALIAS_PATH=\"/usr/local/share/locale\" -DUSEJAVA=0 -DGETTEXTJAR=\"/usr/local/share/gettext/gettext.jar\" -DLIBDIR=\"/usr/local/lib\" -DGETTEXTDATADIR=\"/usr/local/share/gettext\" -DPROJECTSDIR=\"/usr/local/share/gettext/projects\" -DEXEEXT=\"\" -DHAVE_CONFIG_H -I. -I..  -I. -I. -I.. -I.. -I../libgrep -I../gnulib-lib -I../gnulib-lib -I../intl -I../../gettext-runtime/intl -DINSTALLDIR=\"/usr/local/bin\"   -g -O2 -c -o kusarymash.o kusarymash.c
gcc -g -O2 kusarymash.o msgunfmt-read-mo.o msgfmt-write-mo.o ../../gettext-runtime/intl/msgunfmt-hash-string.o  ./.libs/libgettextsrc.so /home/keremet/compile/gettext/gettext/gettext-tools/gnulib-lib/.libs/libgettextlib.so -lxml2 /home/keremet/compile/gettext/gettext/libtextstyle/lib/.libs/libtextstyle.so -lm -lncurses -lc -Wl,-rpath -Wl,/usr/local/lib
for i in `ls /home/keremet/ao_build/rpmbuild/SOURCES/*.mo`; do ./a.out $i $i; done |grep Places
 */
