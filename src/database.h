#ifndef DATABASE_H
#define DATABASE_H

#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

// MySQL C API 헤더
#include <mysql/mysql.h>

// ODBC API 헤더
#include <sql.h>
#include <sqlext.h>

// 사용할 API 종류를 나타내는 열거형 클래스
enum class API {
    C_API,
    ODBC
};

/**
 * @class DatabaseConnector
 * @brief 데이터베이스 연결 및 쿼리 실행을 추상화하는 클래스.
 * MySQL C API를 지원합니다.
 */
class DatabaseConnector {
public:
    // 생성자 및 소멸자
    DatabaseConnector();
    ~DatabaseConnector();

    // 사용할 API를 설정합니다. (ODBC 구현을 고려한 설계)
    void setApi(API api_type);
    
    // 현재 사용 중인 API 타입을 반환합니다.
    API getApi() const;

    // 데이터베이스에 연결합니다.
    bool connect(const std::string& host, const std::string& user, const std::string& password, const std::string& db_name);

    // 데이터베이스 연결을 해제합니다.
    void disconnect();

    // 쿼리를 실행합니다. (주로 SELECT 문에 사용)
    bool executeQuery(const std::string& sql, const std::vector<std::string>& params, std::vector<std::vector<std::string>>& results, std::vector<std::string>& headers);

private:
    API current_api; // 현재 선택된 API

    // --- MySQL C API 관련 멤버 ---
    MYSQL* mysql_conn = nullptr;
    bool connect_c_api(const std::string& host, const std::string& user, const std::string& password, const std::string& db_name);
    bool executeQuery_c_api(const std::string& sql, const std::vector<std::string>& params, std::vector<std::vector<std::string>>& results, std::vector<std::string>& headers);
    void disconnect_c_api();

};

#endif // DATABASE_H