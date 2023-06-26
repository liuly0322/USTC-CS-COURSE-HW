-- 设计一个存储过程 updateReaderID,实现对读者表的 ID 的修改

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

-- TEST

SELECT * FROM Reader;

-- 2 errors

call UPDATE_READER_ID('r1', 'r2', @state);

SELECT @state;

call UPDATE_READER_ID('r1000', 'r2000', @state);

SELECT @state;

call UPDATE_READER_ID('r1', 'r1000', @state);

call UPDATE_READER_ID('r1000', 'r2000', @state);

call UPDATE_READER_ID('r2000', 'r1', @state);

SELECT * FROM Reader;