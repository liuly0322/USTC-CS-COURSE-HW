-- 借书时的存储过程

-- 1. 同一天不允许同一个读者重复借阅同一本读书；

-- 2. 如果该图书存在预约记录，而当前借阅者没有预约，则不允许借阅；

-- 3. 一个读者最多只能借阅 3 本图书，意味着如果读者已经借阅了 3 本图书并且未归还则不允许再借书；

-- 4. 如果借阅者已经预约了该图书，则允许借阅，但要求借阅完成后删除借阅者对该图书的预约记录；

-- 5. 借阅成功后图书表中的 times 加 1；

-- 6. 借阅成功后修改 status。

DROP PROCEDURE IF EXISTS BORROW_BOOK;

Delimiter //

CREATE PROCEDURE BORROW_BOOK(IN BORROW_READER_ID CHAR
(8), BORROW_BOOK_ID CHAR(8), OUT STATE INT) BEGIN 
	DECLARE s INT DEFAULT 0;
	START TRANSACTION;
	-- start code
	IF NOT EXISTS(
	    SELECT id_
	    FROM Reader
	    WHERE
	        id_ = BORROW_READER_ID
	) THEN -- 读者号错误
	SET s = 1;
	ELSEIF NOT EXISTS(
	    SELECT id_
	    FROM Book
	    WHERE
	        id_ = BORROW_BOOK_ID
	)
	OR EXISTS(
	    SELECT *
	    FROM Book
	    WHERE
	        id_ = BORROW_BOOK_ID
	        AND status_ = 1
	) THEN -- 书不可借
	SET s = 2;
	ELSEIF EXISTS(
	    SELECT *
	    FROM Borrow
	    WHERE
	        reader_id = BORROW_READER_ID
	        AND book_id = BORROW_BOOK_ID
	        AND borrow_date = CURDATE()
	) THEN -- 同一天不允许同一个读者重复借阅同一本图书
	SET s = 3;
	ELSEIF EXISTS(
	    SELECT *
	    FROM Reserve
	    WHERE
	        book_id = BORROW_BOOK_ID
	)
	AND NOT EXISTS(
	    SELECT *
	    FROM Reserve
	    WHERE
	        book_id = BORROW_BOOK_ID
	        AND reader_id = BORROW_READER_ID
	) THEN -- 如果该图书存在预约记录，而当前借阅者没有预约，则不允许借阅
	SET s = 4;
	ELSEIF BORROW_READER_ID IN (
	    SELECT reader_id
	    FROM Borrow
	    WHERE return_date IS NULL
	    GROUP BY reader_id
	    HAVING
	        COUNT(*) >= 3
	) THEN -- 已经借了三本书
	SET s = 5;
	ELSE -- 否则允许借阅
	IF EXISTS(
	    SELECT *
	    FROM Reserve
	    WHERE
	        book_id = BORROW_BOOK_ID
	) THEN -- 删除预约记录，减少预约计数
	DELETE FROM Reserve
	WHERE
	    book_id = BORROW_BOOK_ID
	    AND reader_id = BORROW_READER_ID;
	UPDATE Book
	SET
	    reserve_times = reserve_times - 1
	WHERE id_ = BORROW_BOOK_ID;
	END IF;
	INSERT INTO
	    Borrow(
	        reader_id,
	        book_id,
	        borrow_date
	    )
	VALUES (
	        BORROW_READER_ID,
	        BORROW_BOOK_ID,
	        CURDATE()
	    );
	UPDATE Book
	SET
	    borrow_times = borrow_times + 1
	WHERE id_ = BORROW_BOOK_ID;
	UPDATE Book SET status_ = 1 WHERE id_ = BORROW_BOOK_ID;
	END IF;
	-- start error handling
	SET STATE = s;
	IF s = 0 THEN COMMIT;
	ELSE ROLLBACK;
	END IF;
	-- end error handling
	END // 


Delimiter ;

-- TEST

-- 下面测试每一种错误，及正确插入：

-- 1. 读者号错误

CALL BORROW_BOOK('r1000', 'b1', @state);

SELECT @state;

-- 2. 图书号错误（不存在或不可借）

CALL BORROW_BOOK('r1', 'b1000', @state);

SELECT @state;

CALL BORROW_BOOK('r1', 'b1', @state);

SELECT @state;

-- 3. 同一天重复借阅

CALL BORROW_BOOK('r1', 'b7', @state);

CALL BORROW_BOOK('r1', 'b7', @state);

SELECT @state;

-- 4. 有人预约，但自己没预约

CALL BORROW_BOOK('r19', 'b10', @state);

SELECT @state;

-- 5. 借阅者借书超过 3 本

CALL BORROW_BOOK('r23', 'b18', @state);

SELECT @state;

-- 6. 正确插入，可以检查 b17 的 status_ 是否变为 1 及 times 变为 0

CALL BORROW_BOOK('r18', 'b17', @state);

-- 下面的预约记录应该也被删除

CALL BORROW_BOOK('r20', 'b10', @state);

SELECT @state;