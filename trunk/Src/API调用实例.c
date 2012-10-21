int main(void)
{
	char *columnName[3]= {
        "31", "c2", "a3"
    };  	//columnName不能为二维数组
    COLUMN_TYPE columnType[3] = {
        INT, FLOAT, TEXT
    };

    //createDabase, use, createTable等函数失败时会返回-1，我并没有输出error字样
	//show函数不需要额外处理，如果没有可输出的信息，函数会输出"$\n"
	createDatabase("test");
    createDatabase("2test");
    createDatabase("test");
    createDatabase("t3est");
    showDatabase(NOTSORT);
    showDatabase(DESC);
    showDatabase(INCR);

    use("test");
    createTable("test", columnName, columnType, 3);
    createTable("zsdf", columnName, columnType, 3);
    createTable("123", columnName, columnType, 3);
    showTable("test", NOTSORT);
    showTable("test", DESC);
    showTable("test", INCR);
    showTable(NULL, NOTSORT);
    showTable(NULL, DESC);
    showTable(NULL, INCR);

    showColumn("test", NOTSORT);
    showColumn("test", DESC);
    showColumn("test", INCR);

    addColumn("test", "newcolumn", TEXT);
    showColumn("test", NOTSORT);

    rmColumn("test", "31");
    showColumn("test", NOTSORT);

    //showColumnType是我额外添加的函数，可以用来查看每列的类型信息
	showColumnType("test");
    alterColumn("test", "newcolumn", INT);
    showColumnType("test");
    alterColumn("test", "c2", INT);
    showColumnType("test");
	
    truncateTable("test");
    showColumn("test", NOTSORT);

    renameDatabase("test", "test2");
    showDatabase(NOTSORT);

    renameTable("test", "test2");
    showTable(NULL, NOTSORT);

    showTable("test2", NOTSORT);
    drop("test2", "test2");
    showTable("test2", NOTSORT);

    drop("test2", NULL);
    showTable("test2", NOTSORT);
    return 0;
}