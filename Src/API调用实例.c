int main(void)
{
	char *columnName[3]= {
        "31", "c2", "a3"
    };  	//columnName����Ϊ��ά����
    COLUMN_TYPE columnType[3] = {
        INT, FLOAT, TEXT
    };

    //createDabase, use, createTable�Ⱥ���ʧ��ʱ�᷵��-1���Ҳ�û�����error����
	//show��������Ҫ���⴦�����û�п��������Ϣ�����������"$\n"
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

    //showColumnType���Ҷ�����ӵĺ��������������鿴ÿ�е�������Ϣ
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