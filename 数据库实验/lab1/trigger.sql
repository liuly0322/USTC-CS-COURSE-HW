-- 设计一个触发器，实现：当一本书被预约时, 自动将 Book 表中相应图书的 status 修改为 2，并增加 reserve_times

-- 当某本预约的书被借出时或者读者取消预约时, 自动减少 reserve_times

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