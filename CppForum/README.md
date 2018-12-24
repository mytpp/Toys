# CPP论坛文档

每行之间用\n分割

#### 请求报文：  
行号|内容
-|-
1|‘方法’+空格+‘资源’
2|
3|内容（JSON）

方法：GET|POST|PUT|DELETE   
资源：user|board|post|comment

json内容示例：
|       |GET               |POST|PUT|DELETE
-|-|-|-|-
user    |{'id':'123', 'password':'...'}      ||{'admin':'id', 'id':'123', 'status':2, 'board':'C++17'}
board   |
post    |{'board':'C++17'} |{'user':'id', 'board':'name' 'title':'..', 'content':'..'}||{'user':'id', 'post':'id'}
comment |{'post':'id'}      |{'user':'id', 'board':'name', 'post':'id', 'content':'..'}



#### 应答报文：
行号|内容
-|-
1|‘错误码’+空格+‘短语’
2|
3|内容（JSON）

错误码：参考HTTP错误码

|       |GET               |POST|PUT|DELETE
-|-|-|-|-
user|{'status':'..', 'post':5, 'name':'..'}
board|\[{'name':'..', 'moderator':'..'}\]
post|\[{'id':'..', 'author':'name', 'authorId': '..', 'title':'..', 'content':'...', 'date':'..', 'comment':3}\]|{'id':'...'}
comment|\[{'author':'name', 'authorId': '..', 'content':'...', 'date':'..'}\]

