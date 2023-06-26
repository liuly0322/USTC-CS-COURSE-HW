-- 还书时的存储过程

DROP PROCEDURE IF EXISTS RETURN_BOOK;

Delimiter //

CREATE PROCEDURE RETURN_BOOK(IN RETURN_READER_ID CHAR
(8), RETURN_BOOK_ID CHAR(8), OUT STATE INT) BEGIN 
	DECLARE s INT DEFAULT 0;
	START TRANSACTION;
	-- start code
	-- 检查是否存在 borrow 记录，图书是否未还
	IF NOT EXISTS(
	    SELECT *
	    FROM Borrow
	    WHERE
	        reader_id = RETURN_READER_ID
	        AND book_id = RETURN_BOOK_ID
	        AND return_date IS NULL
	) THEN
	    SET s = 1;
	ELSE
	    UPDATE Borrow
	    SET return_date = CURDATE()
	    WHERE
	        reader_id = RETURN_READER_ID
	        AND book_id = RETURN_BOOK_ID
	        AND return_date IS NULL;
	    -- 如果没有预约记录, status_ 设为 0，否则为 2
	    IF NOT EXISTS(
	        SELECT *
	        FROM Reserve
	        WHERE
	            book_id = RETURN_BOOK_ID
	    ) THEN UPDATE Book SET status_ = 0 WHERE id_ = RETURN_BOOK_ID;
	    ELSE UPDATE Book SET status_ = 2 WHERE id_ = RETURN_BOOK_ID;
	    END IF;
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

-- 记录不存在

call RETURN_BOOK('r000', 'b000', @s);

select @s;

-- 还书使得 status_ 为 0

call RETURN_BOOK('r4', 'b6', @s);

select @s;

-- 还书使得 status_ 为 2

call RETURN_BOOK('r23', 'b11', @s);

select @s;