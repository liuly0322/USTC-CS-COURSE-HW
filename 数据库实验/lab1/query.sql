-- 查询读者 Rose 借过的图书（包括已还和未还）的图书号、书名和借期

SELECT
    Book.id_,
    Book.name_,
    Borrow.borrow_date
FROM Book, Borrow, Reader
WHERE
    Reader.name_ = 'Rose'
    AND Reader.id_ = Borrow.reader_id
    AND Borrow.book_id = Book.id_;

-- 查询从没有借过图书也从没有预约过图书的读者号和读者姓名

SELECT
    Reader.id_,
    Reader.name_
FROM Reader
WHERE Reader.id_ NOT IN (
        SELECT reader_id
        from Borrow
        UNION
        SELECT reader_id
        from Reserve
    );

-- 查询被借阅次数最多的作者（注意一个作者可能写了多本书）

SELECT author
FROM Book
GROUP BY author
ORDER BY
    SUM(borrow_times) DESC
LIMIT 1;

-- 查询目前借阅未还的书名中包含“MySQL”的的图书号和书名

SELECT id_, name_
FROM Book
WHERE
    name_ LIKE '%MySQL%'
    AND status_ = 1;

-- 查询借阅图书数目超过 10 本的读者姓名

SELECT Reader.name_
FROM Reader, Borrow
WHERE
    Reader.id_ = Borrow.reader_id
GROUP BY Borrow.reader_id
HAVING
    COUNT(Borrow.book_id) > 10;

-- 查询没有借阅过任何一本 John 所著的图书的读者号和姓名

SELECT
    Reader.id_,
    Reader.name_
FROM Reader
WHERE Reader.id_ NOT IN (
        SELECT reader_id
        FROM Borrow, Book
        WHERE
            Borrow.book_id = Book.id_
            AND Book.author = 'John'
    );

-- 查询 2022 年借阅图书数目排名前 10 名的读者号、姓名以及借阅图书数

SELECT
    Reader.id_,
    Reader.name_,
    COUNT(*)
FROM Reader, Borrow
WHERE
    Reader.id_ = Borrow.reader_id
    AND Borrow.borrow_date >= '2022-01-01'
    AND Borrow.borrow_date < '2023-01-01'
GROUP BY Borrow.reader_id
ORDER BY COUNT(*) DESC
LIMIT 10;

-- 创建一个读者借书信息的视图,该视图包含读者号、姓名、所借图书号、图书名和借期

CREATE OR REPLACE VIEW BORROW_INFO AS 
	 (
	    SELECT
	        Borrow.reader_id,
	        Reader.name_ AS reader_name,
	        Borrow.book_id,
	        Book.name_ AS book_name,
	        Borrow.borrow_date
	    FROM
	        Reader,
	        Book,
	        Borrow
	    WHERE
	        Reader.id_ = Borrow.reader_id
	        AND Book.id_ = Borrow.book_id
	)
; 

-- 并使用该视图查询最近一年所有读者的读者号以及所借阅的不同图书数

SELECT
    reader_id,
    COUNT(DISTINCT book_id)
FROM BORROW_INFO
WHERE
    borrow_date >= DATE_SUB(CURRENT_DATE, INTERVAL 1 YEAR)
GROUP BY reader_id;