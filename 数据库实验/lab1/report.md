# 数据库 Lab1 Report

PB20000180 刘良宇

## 实验目的

通过设计图书馆数据库及查询语句和过程化语言，掌握数据库的设计和实现方法。

## 实验环境

主机环境：

```
$ neofetch
ubuntu@liuly
------------
OS: Ubuntu 20.04.6 LTS on Windows 10 x86_64
Kernel: 5.15.90.1-microsoft-standard-WSL2
Uptime: 5 mins
Packages: 1367 (dpkg), 6 (snap)
Shell: zsh 5.8
Theme: Adwaita [GTK3]
Icons: Adwaita [GTK3]
Terminal: Relay(1354)
CPU: AMD Ryzen 7 4800H with Radeon Graphics (16) @ 2.894GHz
GPU: 20c8:00:00.0 Microsoft Corporation Device 008e
Memory: 610MiB / 7626MiB
```

MySql 拉取自 Dockerhub 的 `mysql:latest`，版本为 8.0.32：

```
sh-4.4# mysql --version
mysql  Ver 8.0.32 for Linux on x86_64 (MySQL Community Server - GPL)
```

通过 Docker 指定转发 3306 端口连接上数据库。

## 实验过程

文件总览：

```
.
├── borrow_book.sql
├── init_table_and_data.sql
├── query.sql
├── return_book.sql
├── trigger.sql
└── update_reader_id.sql
```

### init_table_and_data

创建数据库和表，插入数据。和实验文档要求及提供的一致，注意外键等约束即可。

### query

这个文件中包含了所有的查询语句及借书信息的视图和使用。

### update_reader_id

这个文件中包含了更新读者信息的存储过程。

需要注意读者的 `id` 是主键且受到别的表的外键约束，所以需要临时关闭外键约束，更新完毕后再打开。

这一过程需要保证原子性，所以需要使用事务。

```sql
DROP PROCEDURE IF EXISTS UPDATE_READER_ID;

Delimiter //

-- 0 代表成功，1 代表未找到要更新的读者 ID，2 代表新 ID 已存在或其他错误

CREATE PROCEDURE UPDATE_READER_ID(IN OLD_ID CHAR(8)
, IN NEW_ID CHAR(8), OUT STATE INT) BEGIN
	DECLARE s INT DEFAULT 0;
	DECLARE check_id CHAR(8);
	DECLARE CONTINUE HANDLER FOR NOT FOUND SET s = 1;
	DECLARE CONTINUE HANDLER FOR SQLEXCEPTION SET s = 2;
	-- start transaction
	START TRANSACTION;
	set foreign_key_checks = false;
	-- check if old_id exists
	SELECT id_ FROM Reader WHERE id_ = OLD_ID INTO check_id;
	UPDATE Reader SET id_ = NEW_ID WHERE id_ = OLD_ID;
	UPDATE Borrow SET reader_id = NEW_ID WHERE reader_id = OLD_ID;
	UPDATE Reserve SET reader_id = NEW_ID WHERE reader_id = OLD_ID;
	set foreign_key_checks = true;
	-- check state. commit or rollback
	SET STATE = s;
	IF s = 0 THEN COMMIT;
	ELSE ROLLBACK;
	END IF;
	END //


Delimiter ;
```

### borrow_book

这个文件中包含了借书的存储过程。

主要注意多种错误的判断，分别输出不同的 state。

还有就是可能需要删除预约记录，最后注意更新 status 即可。

### return_book

这个文件中包含了还书的存储过程。需要检查是否存在对应的 borrow 记录。还书完后需要根据当前是否有预约更新 status。

### trigger

这个文件中包含了触发器的定义。都是在行操作时触发。

```sql
CREATE TRIGGER RESERVEBOOK AFTER INSERT ON RESERVE
FOR EACH ROW BEGIN
	UPDATE Book
	SET
	    status = 2,
	    reserve_times = reserve_times + 1
	WHERE id = NEW.book_id;
END;

CREATE TRIGGER CANCELRESERVE AFTER DELETE ON RESERVE
FOR EACH ROW BEGIN
	UPDATE Book
	SET
	    reserve_times = reserve_times - 1
	WHERE id = OLD.book_id;
	-- 如果无人预约了，将 status 改为 0（如果之后借出改为 1 是由借书存储过程负责的）
	IF (
	    SELECT COUNT(*)
	    FROM Reserve
	    WHERE
	        book_id = OLD.book_id
	) = 0 THEN
	UPDATE Book
	SET status = 0
	WHERE id = OLD.book_id;
	END IF;
END;
```

## 实验结果

本实验每个文件都编写了对应的测试样例。可以运行查看结果，均符合预期。

## 总结与思考

本次实验主要是对数据库的设计和实现方法的练习。通过实验，我对数据库的设计和实现方法有了更深的理解，对数据库的使用也更加熟练了。

具体来说，我学会了如何使用 MySQL 的存储过程和触发器，以及如何使用事务来保证原子性。这些都是数据库的重要组成部分，对于数据库的设计和实现都有很大的帮助。

此外，我还尝试使用 Docker 来部署数据库，这样可以避免在本地安装数据库的麻烦，也可以避免数据库的版本不一致导致潜在的问题。
