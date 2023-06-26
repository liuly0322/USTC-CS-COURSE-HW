-- Active: 1683101809663@@127.0.0.1@3306

DROP DATABASE library;

CREATE DATABASE library;

use library;

CREATE TABLE
    Book(
        id_ char(8) PRIMARY KEY,
        name_ varchar(10) NOT NULL,
        author varchar(10),
        price float,
        status_ int DEFAULT 0 Check(
            status_ >= 0
            and status_ <= 2
        ),
        borrow_times int DEFAULT 0,
        reserve_times int DEFAULT 0
    );

CREATE TABLE
    Reader(
        id_ char(8) PRIMARY KEY,
        name_ varchar(10),
        age int,
        address_ varchar(20)
    );

CREATE TABLE
    Borrow(
        book_id char(8),
        reader_id char(8),
        borrow_date date,
        return_date date,
        Constraint PK Primary Key(
            book_id,
            reader_id,
            borrow_date
        ),
        Constraint Borrow_FK_book_id Foreign Key(book_id) References Book(id_),
        Constraint Borrow_FK_reader_id Foreign Key(reader_id) References Reader(id_)
    );

CREATE TABLE
    Reserve(
        book_id char(8),
        reader_id char(8),
        reserve_date date DEFAULT (CURRENT_DATE),
        take_date date,
        Constraint PK Primary Key(
            book_id,
            reader_id,
            reserve_date
        ),
        Constraint Reserve_FK_book_id Foreign Key(book_id) References Book(id_),
        Constraint Reserve_FK_reader_id Foreign Key(reader_id) References Reader(id_),
        Constraint Check_date Check(take_date >= reserve_date)
    );

use library;

# 插入书籍
insert into Book value('b1', '数据库系统实现', 'Ullman', 59.0, 1, 8, 0);
insert into Book value('b2', '数据库系统概念', 'Abraham', 59.0, 1, 5, 0);
insert into Book value('b3', 'C++ Primer', 'Stanley', 78.6, 1, 6, 0);
insert into Book value('b4', 'Redis设计与实现', '黄建宏', 79.0, 1, 4, 0);
insert into Book value('b5', '人类简史', 'Yuval', 68.00, 1, 6, 0);
insert into Book value('b6', '史记(公版)', '司马迁', 220.2, 1, 5, 0);
insert into Book value('b7', 'Oracle编程艺术', 'Thomas', 43.1, 0, 4, 0);
insert into Book value('b8', '分布式系统及其应用', '邵佩英', 30.0, 0, 2, 0);
insert into Book value('b9', 'Oracle管理', '张立杰', 51.9, 0, 5, 0);
insert into Book value('b10', '数理逻辑', '汪芳庭', 22.0, 2, 6, 2);
insert into Book value('b11', '三体', '刘慈欣', 23.0, 1, 8, 1);
insert into Book value('b12', 'Fun python', 'Luciano', 354.2, 0, 3, 0);
insert into Book value('b13', 'Learn SQL', 'Seyed', 23.0, 1, 3, 0);
insert into Book value('b14', 'Perl&MySQL', '徐泽平', 23.0, 1, 3, 0);
insert into Book value('b15', '司马迁的故事', '黄永年', 34.80, 0, 2, 0);
insert into Book value('b16', '中国2185', '刘慈欣', 218.5, 1, 7, 0);
insert into Book value('b17', '高性能MySQL', 'Botros', 20.00, 0, 0, 0);  
insert into Book value('b18', '深度学习', 'John', 111.5, 0, 3, 0);
insert into Book value('b19', 'HowWeThink', 'John', 79.8, 1, 5, 0);

# 插入读者
insert into Reader value('r1', '王林', 18, '中国科学技术大学东校区');
insert into Reader value('r2', 'Rose', 22, '中国科学技术大学北校区');
insert into Reader value('r3', '罗永平', 23, '中国科学技术大学西校区');
insert into Reader value('r4', 'Mora', 26, '中国科学技术大学北校区');
insert into Reader value('r5', '汤晨', 22, '先进科学技术研究院');
insert into Reader value('r6', '李一一', 18, '中国科学技术大学东校区');
insert into Reader value('r7', '王二狗', 22, '中国科学技术大学北校区');
insert into Reader value('r8', '赵四', 23, '中国科学技术大学西校区');
insert into Reader value('r9', '魏心', 26, '中国科学技术大学北校区');
insert into Reader value('r10', '汤大晨', 22, '先进科学技术研究院');
insert into Reader value('r11', '李平', 18, '中国科学技术大学东校区');
insert into Reader value('r12', 'Lee', 22, '中国科学技术大学北校区');
insert into Reader value('r13', 'Jack', 23, '中国科学技术大学西校区');
insert into Reader value('r14', 'Bob', 26, '中国科学技术大学北校区');
insert into Reader value('r15', '李晓', 22, '先进科学技术研究院');
insert into Reader value('r16', '王林', 18, '中国科学技术大学东校区');
insert into Reader value('r17', 'Mike', 22, '中国科学技术大学北校区');
insert into Reader value('r18', '范维', 23, '中国科学技术大学西校区');
insert into Reader value('r19', 'David', 26, '中国科学技术大学北校区');
insert into Reader value('r20', 'Vipin', 22, '先进科学技术研究院');
insert into Reader value('r21', '林立', 18, '中国科学技术大学东校区');
insert into Reader value('r22', '张悟', 22, '中国科学技术大学北校区');
insert into Reader value('r23', '袁平', 23, '中国科学技术大学西校区');

# 插入借书记录
insert into Borrow value('b1','r1',  '2022-03-12', '2022-04-07');
insert into Borrow value('b2','r1',  '2022-04-14', '2022-06-19');
insert into Borrow value('b3','r1',  '2018-01-01', '2018-01-02');
insert into Borrow value('b4','r1',  '2018-01-03', '2018-01-04');
insert into Borrow value('b5','r1',  '2018-01-05', '2018-01-06');
insert into Borrow value('b6','r1',  '2018-01-07', '2018-01-08');
insert into Borrow value('b7','r1',  '2018-01-09', '2018-01-10');
insert into Borrow value('b8','r1',  '2018-01-11', '2018-01-12');
insert into Borrow value('b9','r1',  '2018-01-13', '2018-01-14');
insert into Borrow value('b10','r1',  '2018-01-15', '2018-01-16');
insert into Borrow value('b14','r1',  '2022-01-12', '2022-03-01');

insert into Borrow value('b1', 'r2', '2022-02-22', '2022-03-10');
insert into Borrow value('b2', 'r2', '2022-02-22', '2022-04-10');
insert into Borrow value('b11', 'r2', '2022-01-11', '2022-02-10');
insert into Borrow value('b16', 'r2', '2022-01-11', '2022-02-13');
insert into Borrow value('b19', 'r2', '2023-04-8', NULL);

insert into Borrow value('b1', 'r3', '2022-04-14', '2022-06-19');
insert into Borrow value('b4', 'r3', '2022-01-14', '2022-04-09');
insert into Borrow value('b7', 'r3', '2022-04-02', '2022-04-09');
insert into Borrow value('b18', 'r3', '2022-01-14', '2022-04-09');

insert into Borrow value('b6', 'r4', '2023-03-31', NULL);
insert into Borrow value('b12', 'r4', '2022-03-31', '2022-06-19');
insert into Borrow value('b15', 'r4', '2022-02-02', '2022-03-02');

insert into Borrow value('b4', 'r5', '2023-04-10', NULL);
insert into Borrow value('b16','r5',  '2022-06-15', '2022-06-30');
insert into Borrow value('b19', 'r5', '2022-06-15', '2022-06-16');

insert into Borrow value('b3', 'r6', '2022-01-10', '2022-5-01');
insert into Borrow value('b3', 'r6', '2023-01-10', NULL);
insert into Borrow value('b13', 'r6', '2022-01-10', '2022-2-10');
insert into Borrow value('b14', 'r6', '2023-01-10', '2023-02-28');

insert into Borrow value('b1', 'r7', '2022-01-01', '2022-01-20');
insert into Borrow value('b16', 'r7', '2022-01-01', '2022-01-09');
insert into Borrow value('b19', 'r7', '2022-01-01', '2022-02-14');

insert into Borrow value('b8', 'r8', '2022-01-10', '2022-02-19');
insert into Borrow value('b9','r8',  '2022-01-10', '2022-02-19');
insert into Borrow value('b10', 'r8', '2022-01-10', '2022-02-19');

insert into Borrow value('b1', 'r9', '2022-06-20', '2022-06-30');
insert into Borrow value('b2', 'r9', '2022-06-20', '2022-06-30');
insert into Borrow value('b19', 'r9', '2022-03-01', '2022-03-22');

insert into Borrow value('b1', 'r11', '2022-07-01', '2022-07-05');
insert into Borrow value('b3', 'r11',  '2022-07-01', '2022-07-05');
insert into Borrow value('b5', 'r11', '2022-07-01', '2022-07-07');
insert into Borrow value('b7', 'r11', '2022-07-07', '2022-07-31');
insert into Borrow value('b11','r11',  '2021-01-12', '2021-01-31');
insert into Borrow value('b16', 'r11', '2021-01-31', '2021-02-26');

insert into Borrow value('b3', 'r12', '2022-07-10', '2022-07-31');
insert into Borrow value('b11', 'r12', '2021-02-01', '2021-02-26');
insert into Borrow value('b16', 'r12', '2021-02-26', '2021-03-31');

insert into Borrow value('b18', 'r13', '2022-07-01', '2022-08-01');
insert into Borrow value('b19', 'r13', '2022-07-01', '2022-08-01');

insert into Borrow value('b5', 'r14', '2023-01-10', NULL);
insert into Borrow value('b1', 'r14', '2023-01-10', NULL);

insert into Borrow value('b9', 'r15', '2021-04-19', '2021-08-19');
insert into Borrow value('b2', 'r15', '2022-12-01', NULL);

insert into Borrow value('b1', 'r16', '2019-09-11', '2019-11-09');
insert into Borrow value('b2', 'r16', '2020-01-08', '2020-02-08');
insert into Borrow value('b3', 'r16', '2020-01-08', '2020-02-08');
insert into Borrow value('b4', 'r16', '2020-01-08', '2020-02-08');
insert into Borrow value('b5', 'r16', '2020-03-08', '2020-04-08');
insert into Borrow value('b7', 'r16', '2020-03-08', '2020-04-08');
insert into Borrow value('b9', 'r16', '2021-10-10', '2021-12-19');
insert into Borrow value('b10', 'r16', '2022-07-24', '2022-09-21');
insert into Borrow value('b11', 'r16', '2020-05-08', '2020-09-08');
insert into Borrow value('b12', 'r16', '2019-09-01', '2019-09-10');
insert into Borrow value('b15', 'r16', '2019-09-11', '2019-11-09');
insert into Borrow value('b16', 'r16', '2019-09-11', '2019-11-09');
insert into Borrow value('b18', 'r16', '2019-09-11', '2019-11-09');

insert into Borrow value('b9', 'r17', '2022-07-10', '2022-08-22');
insert into Borrow value('b11','r17',  '2021-12-12', '2022-01-01');

insert into Borrow value('b12', 'r18', '2021-10-10', '2021-12-19');
insert into Borrow value('b13', 'r18', '2021-10-10', '2021-12-19');

insert into Borrow value('b5','r19',  '2019-01-01', '2019-01-11');
insert into Borrow value('b5','r19',  '2019-02-12', '2019-03-07');
insert into Borrow value('b6','r19',  '2019-03-08', '2019-04-07');
insert into Borrow value('b6', 'r19', '2019-05-06', '2019-05-09');
insert into Borrow value('b6','r19',  '2019-07-12', '2019-09-07');
insert into Borrow value('b10','r19',  '2019-02-12', '2019-03-07');
insert into Borrow value('b10','r19',  '2019-04-08', '2019-05-05');
insert into Borrow value('b10','r19',  '2019-09-12', '2019-11-07');
insert into Borrow value('b11','r19',  '2019-01-01', '2022-01-11');
insert into Borrow value('b11','r19',  '2019-02-12', '2022-03-07');
insert into Borrow value('b13', 'r19', '2022-12-10', NULL);

insert into Borrow value('b11', 'r23', '2022-12-17', NULL);
insert into Borrow value('b14', 'r23', '2022-12-17', NULL);
insert into Borrow value('b16', 'r23', '2022-12-17', NULL);

# 插入预约记录
insert into Reserve value('b10', 'r20', '2023-02-17', NULL);
insert into Reserve value('b11', 'r20', '2023-02-17', NULL);

insert into Reserve value('b10', 'r21', '2023-02-18', NULL);