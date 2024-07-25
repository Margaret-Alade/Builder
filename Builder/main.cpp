//
//  main.cpp
//  Builder
//
//  Created by Маргарет  on 16.07.2024.
//

#include <iostream>
#include <pqxx/pqxx>


struct Studentdata {
    std::string table_name = "";
    std::string column_name = "*";
    std::map<std::string, std::string> column_where_data;
    std::string column_where = "";
};


class SqlSelectQueryBuilder {
public:
    SqlSelectQueryBuilder() = default;
    
    SqlSelectQueryBuilder& AddColumn(std::string column) {
        if (studentdata.column_name != "*") {
            studentdata.column_name += ", " + column;
        } else {
            studentdata.column_name = column;
        }
        return *this;
    }
    
    SqlSelectQueryBuilder& AddFrom(std::string table) {
        studentdata.table_name = table;
        return *this;
    }
    
    SqlSelectQueryBuilder& AddWhereOr(std::string key, std::string value) {
        
        if (!studentdata.column_where_data.empty()) {
            studentdata.column_where_data[key] = value;
            studentdata.column_where += " OR " +  key + "=" + value;
        } else {
            studentdata.column_where_data[key] = value;
            studentdata.column_where = key + "=" + value;
        }
        return *this;
    }
    
    SqlSelectQueryBuilder& AddWhereAnd(std::string key, std::string value) {
        
        if (!studentdata.column_where_data.empty()) {
            studentdata.column_where_data[key] = value;
            studentdata.column_where += " AND " +  key + "=" + value;
        } else {
            studentdata.column_where_data[key] = value;
            studentdata.column_where = key + "=" + value;
        }
        return *this;
    }
    
    void BuildQuery(pqxx::connection& conn) {
        pqxx::work work1(conn);
        
        if (studentdata.column_where_data.empty()) {
            auto result_rows = work1.query<int, std::string, std::string, std::string>("SELECT " + studentdata.column_name + " FROM " + studentdata.table_name );
            for (auto & result : result_rows) {
                std::cout << get<0>(result) << ", " << get<1>(result) << ", " << get<2>(result) << ", " << get<3>(result) << "\n";
            }
            
        } else {
            auto result_rows = work1.query<int, std::string, std::string, std::string>("SELECT " + studentdata.column_name + " FROM " + studentdata.table_name + " WHERE " + studentdata.column_where);
            
            for (auto & result : result_rows) {
                std::cout << get<0>(result) << ", " << get<1>(result) << ", " << get<2>(result) << ", " << get<3>(result) << "\n";
            }
        }
        
        work1.commit();
    }
    
private:
    Studentdata studentdata;
};

void createTable(pqxx::connection& conn) {
    pqxx::work work1(conn);
    work1.exec("CREATE TABLE IF NOT EXISTS Students (id serial PRIMARY KEY, name varchar(20) NOT NULL, surname varchar(20) NOT NULL, number varchar(20) NOT NULL);");
    work1.commit();
}

int main(int argc, const char * argv[]) {
    
    try {
        pqxx::connection conn (
                              "host=MacBook-Air-Margaret.local "
                              "port=5432 "
                              "dbname=builder "
                              "user=postgres "
                              "password=8563"
                              );
        
        std::cout << "Connection is established" << "\n";
        
//        createTable(conn);
//        
//        pqxx::work work1(conn);
//        work1.exec("INSERT INTO Students(name, surname, number) "
//                   "VALUES('Николай', 'Орлов', '+79058035621')");
//        work1.commit();
//        
//        pqxx::work work2(conn);
//        work2.exec("INSERT INTO Students(name, surname, number) "
//                   "VALUES('Cветлана', 'Китаева', '+79789032233')");
//        work2.commit();
//        
//        pqxx::work work3(conn);
//        work3.exec("INSERT INTO Students(name, surname, number) "
//                   "VALUES('Вячеслав', 'Тимофеев', '+79917436741')");
//        work3.commit();
        
        SqlSelectQueryBuilder query;
        
        query.AddColumn("id")
             .AddColumn("name")
             .AddColumn("surname")
             .AddColumn("number")
             .AddFrom("Students")
             .AddWhereOr("name", "'Николай'")
             .AddWhereOr("surname", "'Китаева'");
        query.BuildQuery(conn);
        
        return 0;
        
    } catch (std::exception& e) {
        std::cout << "Exception: " << e.what() << "\n";
    }
    return 0;
}
