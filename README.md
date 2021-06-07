# DBMS

**DBMS** - special-purpose (universities) database management system. The work is still in progress.

All commands are case insensitive (but not names!). Commands are also have to end with semicolon (';'). Here are several guide aliases for custom values: <db> - database name, <tb> - table name, <cl> - column name, <val> - value, <sgn> - sign (=|>|<). For more information type *help* in program.

Here are the relational database project:

![database](./img/DB.png)

## Проверка выполнения задания

### Запуск

При запуске программа ищет файл *.config*. Файл ищется в папке, из которой запускается исполняемый файл, или же на одну директорию выше. После нахождения вышеуказанного файла директория, в которой он располагается, обозначается корневой. В ней будут располагаться папка с базами данных *.database*. В ней же будет искаться папка *scripts* со скриптами, запускаемыми командой *source*. Если файл *.config* не будет найден, программа не запустится. Пожалуйста, следите за тем, чтобы вложенность исполняемого файла по отношению к *.config*  не превышала 1.
<br><br>

### Общая информация

После запуска вы увидите терминал DBMS:
```
DBMS [(none)]> 
```
Впредь для обозначения строки ввода DBMS будет использоваться символ '>'.

*None* означает, что ни одна из баз данных не используется в данный момент. Чтобы открыть БД введите команду:
```
> USE < название БД >;
```
( после запуска будет доступна лишь системная БД ). 
После открытия БД, на месте *none* будет ее название.

Для выхода из программы используйте:
```
> quit
```
Или же просто 'q'.

Команды, выполняемые внутри терминала DBMS, регистронезависимые. Однако названия таковыми *не являются*! Все команды также должны заканчиваться ';'. Команды могут быть многострочными.
<br><br>

### Выполнение скриптов (эскпериментальная БД)

Для развёртывания экспериментальной базы данных используйте команду:
```
> SOURCE example_db.dbms;
```
В случае успешного выполнения появится надпись "Success!".

Скрипты располагаются в папке *scripts*. Вы также можете написать свои, используя команды терминала DBMS (более подробную информацию о них можно посмотреть c помощью команды  **help** внутри DBMS). Циклы и условные конструкции отсутствуют. Скрипт прекратит свое выполнение на команде, которая будет не выполнена или выполнена неверно, выведя в терминал информацию о причинах своей остановки, а также саму команду, которая не была выполнена успешно. Комментарии в скриптах начинаются с символа '#'.
<br><br>

### Команды интерфейса DBMS
<br>

1. **Создание однотипных и гибридных БД**
    
    Чтобы создать однотипную базу данных, используйте команду 
    ```
    > CREATE DATABASE < название бд > (type=common);
    ```
    При создании такой БД тип может быть опущен. 

    Для создания гибридной БД:  
    ```
    > CREATE DATABASE < название бд > (type=hybrid);
    ```
    Запрещено создавать БД с одинаковыми названиями. 
<br>

2. **Вывод на экран списка существующих БД**
    
    Для просмотра списка всех существующих баз данных используется команда:
    ```
    > SHOW DATABASES;
    ```
    В выводе команды вы всегда увидите базу данных под названием *information_schema*. Она создается при первом запуске программы. В ней хранятся информация о всех существующих БД. При повреждении этого файла открыть другие базы данных будет невозможно, т.к. первичная информация при открытии БД всегда считывается отсюда. Ее невозможно (через терминал) удалить или изменить.
<br>

3. **Удаление БД**
    
    Удаление базы данных производится посредством команды:
    ```
    > DROP DATABASE <название бд>;
    ```
    Производится удаление файла БД с диска. После чего удаляется информация о БД из *information_schema*. Если в момент удаления БД была открыта, то текущее состояние DBMS меняется с названия БД на *none*. Удалить *information_schema* нельзя.
<br>

4. **Переименование БД**

    Чтобы сменить название БД, используйте:
    ```
    > RENAME DATABASE < текущее название > TO < новое название >;
    ```
    Меняется как название самого файла на диске, так и информация внутри *information_schema*.
<br>

5. **Открытие БД**

     ```
     > USE < название БД >;
     ```
     После открытия БД, в нее можно будет добавлять записи, удалять, изменять и просматривать их.Все остальные действия(указанные выше) не требуют открытия БД.
<br>

6. **Сохранение БД**

    Ручное сохранение БД отсутствует. Она сохраняется каждый раз, когда меняется текущая БД посредством команды **USE**, или же при выходе из программы. В случае непредвиденного закрытия изменения в БД могут не сохраниться.

7. **Добавление записей в БД**

    База данных построена на таблицах. В случае однотипной БД: faculties, departments, disciplines_dep (см. картинку выше), гибридной: те же, что для однотипной + base_org, disciplines_borg. Записи в БД добавляются посредством добавления записей в таблицы.

    Чтобы увидеть список таблиц, доступных в БД, введите:
    ```
    > SHOW TABLES;
    ```
    До этого уже должна быть открыта какая-либо БД.

    Чтобы увидеть информацию о таблице используйте:
    ```
    > DESCRIBE < имя таблицы >;
    ```
    Имя таблицы берется из *SHOW TABLES;*.

    Для добавления записи в таблицу:
    ```
    > INSERT < имя таблицы > VALUES (< значение 1 >, ... < значение n >);
    ```
    Значения вводятся в соответсвии с типами данных, полученных из команды *DESCRIBE*. Если оно *str*, то значение должно быть в кавычках. **Значение id вводить не нужно!** Оно задается автоматически автоинкрементом.

    Пример:

    ```
    > SOURCE example_db.dbms;
    Reading script...
    Success!

    # На данный момент БД MGTU уже выбрана
    > SHOW TABLES;
    +------------------+
    | tables           |
    +------------------+
    |faculties         |
	|departments       |
	|disciplines_dep   |
	|base_org          |
	|disciplines_borg  |
	+------------------+

    > DESCRIBE FACULTIES;
	+---------------------------+
	| Columns           | Types |
	+---------------------------+
	|fac_id             |int    |
	|fac_name           |str    |
	|nuc_name           |str    |
	|num_dep            |int    |
	|is_hybrid          |int    |
	+---------------------------+

    > INSERT faculties VALUES ('new_fac', 'new_nuc', 7, 0);
    ```
<br>

8. **Удаление записей в БД**
    
    Удаление кортежа из таблицы может осуществляться по имени или же по id:
    ```
    # Вместо id может быть <...> name=< имя кортежа (2-ой столбик) >
    > DELETE FROM < название таблицы > WHERE id=< значение id > 
    ```
    Между id, '=' и значением (или все то же самое для имени) **не должно быть пробелов**.
    Удаление происходит **рекурсивно**: будут удалены все элементы, зависящие от удаляемого, и в дургих таблицах.

    Пример:
    ```
    > DELETE FROM faculties WHERE name='new_fac';
    ```
<br>

9. **Вывод на экран записей БД**

    Вывод всей информации, содержащейся в таблице осуществляется командой:
    ```
    > SELECT * FROM < название таблицы >;
    ```

    Вместо '*' могут быть указаны названия колонок, полученные из *DESCRIBE*:
    Например:
    ```
    > SELECT fac_id, fac_name from faculties;
    ```
<br>

10. **Сортировка записей**

    Сортировка осуществляется также с помощью *select*:
    ```
    > SELECT ... FROM < название таблицы > ORDER BY < название колонки > ASC;
    ```
    Вместо *ASC* (возрастающий) может стоять *DESC* (убывающий). Сортировка происходит по любому столбцу таблицы.
<br>

11. **Выборки**

    Выборки тоже осуществляются с помощью *select*:
    ```
    > SELECT ... FROM < название таблицы > WHERE <название колонки><знак><значение>;
    ```
    *Where* идет перед *order by* в *select*. На месте знака могут стоять '=', '>', '<'.
    Между названием колонки, знаком и значением **не должно быть запятых**.

    Например:
    ```
    > SELECT * FROM disciplines_dep where num_teach<15;
    ```
<br>

12. **Сохранение выборки**

    Сохранение выборки производится добавлением *write* в конец *select*:
    ```
    > SELECT ... FROM ... WHERE ... ORDER BY ... WRITE;
    ```
    Сохранение происходит **рекурсивно**. *Where* и *order by* не обязательны. Все аргументы *select* могут идти в любом наборе (т.е. только write или же where и write и т.д.). Главное - соблюдать порядок их следования.
<br>

13. **Редактирование записей**
    
    Изменение записи производится посредством команды:
    ```
    > UPDATE < название таблицы> SET < название столбца 1 >=< значение >, ... < название столбца n >=< значение n > WHERE id=< значение id>;
    ```

    Могут изменяться от 1 до n столбиков кортежа. Вместо id может стоять имя.

    Пример:
    ```
    > UPDATE disciplines_dep SET dis_name='new_dis', num_teach=10 where name='IU_dis_0';
    ```
<br>

### Индивидуальные функции, выборки, сортировки
<br>
1. **Функции**
    1. Количество уникальных дисциплин
    ```
    > disciplines_num <название факультета>;
    ```
    2. Поиск факультета
    ```
    > faculty_info <название факультета>;
    ```
<br>
2. **Правила сортировки**
    1. Сортировка по названию факультета
    ```
    > select * from faculties order by fac_name asc;
    ```
    2. Сортировка по числу кафедр
    ```
    > select * from faculties order by num_dep asc;
    ```
<br>
3. **Правила выборки**
    1. Факультеты, имеющие больше n кафедр
    ```
    > select * from faculties where num_dep>3;
    ```
    2. Факультет, ведущий какую-либо дисциплину
    ```
    > which_faculty < название предмета >;
    ```
