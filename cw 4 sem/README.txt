     Обработка параметров командной строки
     Аргумент 0 - путь к приложению
     Аргумент 1 - путь к файлу с командами
          (если равен "--dialog", то интерактивный режим с пользователем)
     Аргумент 2 - путь к файлу с БД (необязательно)
          (если не задан - структуры и данные размещаются в оперативной памяти приложения)
          (если задан - структуры и данные размещаются в файловой системе)
     Если нет 3-х аргументов, программу завершаем
    
     Примеры запуска:
          main commands.txt - структуры и данные размещаются в оперативной памяти приложения
          main commands.txt storage.txt - структуры и данные размещаются в файловой системе (storage.txt)
          main --dialog - диалоговый режим + структуры и данные размещаются в оперативной памяти приложения

     Допустимые команды (<> - скобки означают произвольное значение):
    
     Строка с ' - комментарий
    
     PRINT - печать коллекции
    
     ADD <poolName> <schemeName> <collectionName> <key> <CustomerId> <CustomerName> <CustomerBalance> -
         добавить клиента <CustomerId> <CustomerName> <CustomerBalance> по ключу <key>
         в коллекцию <poolName> <schemeName> <collectionName>
    
     GET <poolName> <schemeName> <collectionName> <key> -
         получить клиента <CustomerId> <CustomerName> <CustomerBalance> по ключу <key>
     GET <minbound> <maxbound> -
         получить клиентов по ключу <key> в диапазоне <minbound> <maxbound> (по всем пулам, схемам, коллекциям)
    
     DELETE <poolName> - удалить пул и именем <poolName>
     DELETE <poolName> <schemeName> - удалить схему <schemeName> в пуле <poolName>
     DELETE <poolName> <schemeName> <collectionName> - удалить коллекцию <collectionName>
                                                       в схеме <schemeName> в пуле <poolName>
     DELETE <poolName> <schemeName> <collectionName> <key> - удалить объект по ключу <key>
                                                             в коллекции <collectionName>
                                                             в схеме <schemeName> в пуле <poolName>
     LOAD <filename> - загрузить БД из файла <filename>
     SAVE <filename> - сохранить БД в файл <filename>

     Реализованные доп. задания: 1, 5