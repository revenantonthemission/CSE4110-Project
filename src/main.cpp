#include "database.h"
#include <limits>
#include <iomanip>

// --- Helper Functions  ---
void displayMenu(API currentApi) {
    std::cout << "\n=========== Convenience Store DB System (Project 1 Queries) ===========\n";
    std::cout << "1. TYPE 1\n";
    std::cout << "2. TYPE 2\n";
    std::cout << "3. TYPE 3\n";
    std::cout << "4. TYPE 4\n";
    std::cout << "5. TYPE 5\n";
    std::cout << "6. TYPE 6\n";
    std::cout << "7. TYPE 7\n";
    std::cout << "----------------------------------------------------------------------\n";
    std::cout << "0. Exit\n";
    std::cout << "======================================================================\n";
    std::cout << "Enter your choice: ";
}

void printResults(const std::vector<std::string>& headers, const std::vector<std::vector<std::string>>& results) {
    if (results.empty()) {
        std::cout << "-> No results found." << std::endl;
        return;
    }
    for (const auto& header : headers) std::cout << std::left << std::setw(22) << header;
    std::cout << "\n" << std::string(headers.size() * 22, '-') << std::endl;

    for (const auto& row : results) {
        for (const auto& field : row) {
            std::cout << std::left << std::setw(22) << (field.empty() ? "NULL" : field);
        }
        std::cout << "\n";
    }
}

std::string getInput(const std::string& prompt) {
    std::string input;
    std::cout << prompt;
    if(std::cin.peek() == '\n') std::cin.ignore();
    std::getline(std::cin, input);
    return input;
}

// --- Query Functions ---

// 1. Product Availability
void queryProductAvailability(DatabaseConnector& db) {
    std::string keyword = getInput("Which stores currently carry a certain product (by UPC, name, or brand), and how much inventory do they have?\nEnter program identifier (UPC, name or brand): ");
    std::string sql = "SELECT S.name as Store, P.name as Product, P.brand, SP.inventory_level as Stock "
                      "FROM Store S JOIN Store_Product SP ON S.store_id = SP.store_id "
                      "JOIN Product P ON SP.product_upc = P.product_upc "
                      "WHERE P.name LIKE ? OR P.brand LIKE ?;";
    std::string like_keyword = "%" + keyword + "%";
    std::vector<std::string> params = {like_keyword, like_keyword};
    std::vector<std::vector<std::string>> results;
    std::vector<std::string> headers;
    if (db.executeQuery(sql, params, results, headers)) printResults(headers, results);
}

// 2. Top-Selling Items
void queryTopSelling(DatabaseConnector& db) {
    std::cout << "Which products have the highest sales volume in each store over the past month?" << std::endl;
    // 이 쿼리는 윈도우 함수를 사용합니다. MySQL 8.0 이상에서 지원됩니다.
    std::string sql = "WITH MonthlySales AS ("
                      "  SELECT T.store_id, S.name as store_name, P.name as product_name, SUM(TD.quantity) as total_quantity, "
                      "  ROW_NUMBER() OVER(PARTITION BY T.store_id ORDER BY SUM(TD.quantity) DESC) as rn "
                      "  FROM Transaction T "
                      "  JOIN Transaction_Detail TD ON T.transaction_id = TD.transaction_id "
                      "  JOIN Product P ON TD.product_upc = P.product_upc "
                      "  JOIN Store S ON T.store_id = S.store_id "
                      "  WHERE T.transaction_time >= DATE_SUB(NOW(), INTERVAL 1 MONTH) "
                      "  GROUP BY T.store_id, S.name, P.name"
                      ") SELECT store_name, product_name, total_quantity FROM MonthlySales WHERE rn <= 3;"; // 매장별 상위 3개
    std::vector<std::vector<std::string>> results;
    std::vector<std::string> headers;
    if (db.executeQuery(sql, {}, results, headers)) printResults(headers, results);
}

// 3. Store Performance
void queryStorePerformance(DatabaseConnector& db) {
    std::cout << "Which store has generated the highest overall revenue this quarter?" << std::endl;
    std::string sql = "SELECT S.name, SUM(T.total_amount) AS total_revenue "
                      "FROM Transaction T JOIN Store S ON T.store_id = S.store_id "
                      "WHERE QUARTER(T.transaction_time) = QUARTER(NOW()) AND YEAR(T.transaction_time) = YEAR(NOW()) "
                      "GROUP BY S.name ORDER BY total_revenue DESC LIMIT 1;";
    std::vector<std::vector<std::string>> results;
    std::vector<std::string> headers;
    if (db.executeQuery(sql, {}, results, headers)) printResults(headers, results);
}

// 4. Vendor Statistics
void queryVendorStats(DatabaseConnector& db) {
    std::cout << "Which vendor supplies the most products across the chain, and how many total units have been sold?" << std::endl;
    std::string sql = "SELECT V.name as vendor_name, "
                      "COUNT(DISTINCT P.product_upc) as num_products_supplied, "
                      "COALESCE(SUM(TD.quantity), 0) as total_units_sold "
                      "FROM Vendor V "
                      "LEFT JOIN Product P ON V.vendor_id = P.vendor_id "
                      "LEFT JOIN Transaction_Detail TD ON P.product_upc = TD.product_upc "
                      "GROUP BY V.name ORDER BY total_units_sold DESC, num_products_supplied DESC;";
    std::vector<std::vector<std::string>> results;
    std::vector<std::string> headers;
    if (db.executeQuery(sql, {}, results, headers)) printResults(headers, results);
}

// 5. Inventory Reorder Alerts
void queryReorderAlerts(DatabaseConnector& db) {
    std::cout << "Which products in each store are below the reorder threshold and need restocking?" << std::endl;
    std::string sql = "SELECT S.name AS store_name, P.name AS product_name, SP.inventory_level, SP.reorder_threshold "
                      "FROM Store_Product SP "
                      "JOIN Store S ON SP.store_id = S.store_id "
                      "JOIN Product P ON SP.product_upc = P.product_upc "
                      "WHERE SP.inventory_level < SP.reorder_threshold;";
    std::vector<std::vector<std::string>> results;
    std::vector<std::string> headers;
    if (db.executeQuery(sql, {}, results, headers)) printResults(headers, results);
}

// 6. Customer Purchase Patterns
void queryCustomerPatterns(DatabaseConnector& db) {
    std::cout << "List the top 3 items that loyalty program customers typically purchase with coffee.\n";
    std::string item = getInput("Enter a product name:");
    // CTE(Common Table Expression)를 사용하여 복잡한 쿼리 작성
    std::string sql = "WITH TargetTransactions AS ("
                      "  SELECT DISTINCT TD.transaction_id "
                      "  FROM Transaction_Detail TD JOIN Product P ON TD.product_upc = P.product_upc "
                      "  WHERE P.name = ?"
                      "), LoyaltyTransactions AS ("
                      "  SELECT T.transaction_id "
                      "  FROM Transaction T JOIN Customer C ON T.customer_id = C.customer_id"
                      ") "
                      "SELECT P.name as purchased_with, COUNT(*) as frequency "
                      "FROM Transaction_Detail TD "
                      "JOIN Product P ON TD.product_upc = P.product_upc "
                      "WHERE TD.transaction_id IN (SELECT transaction_id FROM TargetTransactions) "
                      "  AND TD.transaction_id IN (SELECT transaction_id FROM LoyaltyTransactions) "
                      "  AND P.name != ? "
                      "GROUP BY P.name ORDER BY frequency DESC LIMIT 3;";
    std::vector<std::string> params = {item, item};
    std::vector<std::vector<std::string>> results;
    std::vector<std::string> headers;
    if (db.executeQuery(sql, params, results, headers)) printResults(headers, results);
}

// 7. Franchise vs. Corporate Comparison
void queryFranchiseComparison(DatabaseConnector& db) {
    std::cout << "Among franchise-owned stores, which one offers the widest variety of products, and how does that compare to corporate-owned stores?" << std::endl;
    std::string sql = "SELECT S.ownership_type, S.name as store_name, COUNT(SP.product_upc) AS product_variety "
                      "FROM Store S JOIN Store_Product SP ON S.store_id = SP.store_id "
                      "GROUP BY S.ownership_type, S.name ORDER BY S.ownership_type, product_variety DESC;";
    std::vector<std::vector<std::string>> results;
    std::vector<std::string> headers;
    if (db.executeQuery(sql, {}, results, headers)) printResults(headers, results);
}


// --- Main Function (이전과 동일) ---
int main() {
    DatabaseConnector db;
    
    // 🚨 데이터베이스 접속 정보를 여기에 입력하세요.
    const std::string host = "127.0.0.1";
    const std::string user = "root";
    const std::string password = "123456789"; // 비밀번호 변경 필수
    const std::string db_name = "store"; 

    if (!db.connect(host, user, password, db_name)) {
        std::cerr << "Initial connection failed. Exiting." << std::endl;
        return 1;
    }
     std::cout << "-> Successfully connected with MySQL C API."<< std::endl;

    int choice;
    do {
        displayMenu(db.getApi());
        std::cin >> choice;
        
        if(std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            choice = -1;
        }

        switch (choice) {
            case 1: queryProductAvailability(db); break;
            case 2: queryTopSelling(db); break;
            case 3: queryStorePerformance(db); break;
            case 4: queryVendorStats(db); break;
            case 5: queryReorderAlerts(db); break;
            case 6: queryCustomerPatterns(db); break;
            case 7: queryFranchiseComparison(db); break;
            case 0: std::cout << "-> Exiting program." << std::endl; break;
            default: std::cout << "-> Invalid choice. Please try again." << std::endl; break;
        }
    } while (choice);

    db.disconnect();
    return 0;
}

// --- 생성자 및 소멸자, API 설정 ---
DatabaseConnector::DatabaseConnector() : current_api(API::C_API) {}

DatabaseConnector::~DatabaseConnector() {
    disconnect();
}

void DatabaseConnector::setApi(API api_type) {
    disconnect(); // API 변경 전 기존 연결 해제
    current_api = api_type;
}

API DatabaseConnector::getApi() const {
    return current_api;
}

// --- 공용 인터페이스 함수 ---
bool DatabaseConnector::connect(const std::string& host, const std::string& user, const std::string& password, const std::string& db_name) {
    return connect_c_api(host, user, password, db_name);
}

void DatabaseConnector::disconnect() {
    disconnect_c_api();
}

bool DatabaseConnector::executeQuery(const std::string& sql, const std::vector<std::string>& params, std::vector<std::vector<std::string>>& results, std::vector<std::string>& headers) {
    return executeQuery_c_api(sql, params, results, headers);
}


// --- MySQL C API 구현부 ---

bool DatabaseConnector::connect_c_api(const std::string& host, const std::string& user, const std::string& password, const std::string& db_name) {
    mysql_conn = mysql_init(nullptr);
    if (!mysql_conn) {
        std::cerr << "C_API Error: mysql_init() failed." << std::endl;
        return false;
    }
    if (!mysql_real_connect(mysql_conn, host.c_str(), user.c_str(), password.c_str(), db_name.c_str(), 0, nullptr, 0)) {
        std::cerr << "C_API Error: mysql_real_connect() failed: " << mysql_error(mysql_conn) << std::endl;
        mysql_close(mysql_conn);
        mysql_conn = nullptr;
        return false;
    }
    // UTF-8 인코딩 설정
    mysql_set_character_set(mysql_conn, "utf8");
    return true;
}

void DatabaseConnector::disconnect_c_api() {
    if (mysql_conn) {
        mysql_close(mysql_conn);
        mysql_conn = nullptr;
    }
}

bool DatabaseConnector::executeQuery_c_api(const std::string& sql, const std::vector<std::string>& params, std::vector<std::vector<std::string>>& results, std::vector<std::string>& headers) {
    if (!mysql_conn) {
        std::cerr << "C_API Error: Not connected." << std::endl;
        return false;
    }
    MYSQL_STMT* stmt = mysql_stmt_init(mysql_conn);
    if (!stmt) {
        std::cerr << "C_API Error: mysql_stmt_init() failed" << std::endl;
        return false;
    }
    if (mysql_stmt_prepare(stmt, sql.c_str(), sql.length())) {
        std::cerr << "C_API Error: mysql_stmt_prepare() failed: " << mysql_stmt_error(stmt) << std::endl;
        mysql_stmt_close(stmt);
        return false;
    }

    std::vector<MYSQL_BIND> bind_params(params.size());
    if (!params.empty()) {
        for (size_t i = 0; i < params.size(); ++i) {
            bind_params[i] = {};
            bind_params[i].buffer_type = MYSQL_TYPE_STRING;
            bind_params[i].buffer = (char*)params[i].c_str();
            bind_params[i].buffer_length = params[i].length();
        }
        if (mysql_stmt_bind_param(stmt, bind_params.data())) {
            std::cerr << "C_API Error: mysql_stmt_bind_param() failed: " << mysql_stmt_error(stmt) << std::endl;
            mysql_stmt_close(stmt);
            return false;
        }
    }
    if (mysql_stmt_execute(stmt)) {
        std::cerr << "C_API Error: mysql_stmt_execute() failed: " << mysql_stmt_error(stmt) << std::endl;
        mysql_stmt_close(stmt);
        return false;
    }

    MYSQL_RES* result_metadata = mysql_stmt_result_metadata(stmt);
    if (!result_metadata) {
        mysql_stmt_close(stmt);
        return true; 
    }

    int num_fields = mysql_num_fields(result_metadata);
    MYSQL_FIELD* fields = mysql_fetch_fields(result_metadata);
    headers.clear();
    for(int i = 0; i < num_fields; i++) headers.push_back(fields[i].name);

    std::vector<std::vector<char>> result_buffers(num_fields, std::vector<char>(512));
    std::vector<unsigned long> lengths(num_fields);
    std::vector<MYSQL_BIND> bind_results(num_fields);
    results.clear();
    
    for (int i = 0; i < num_fields; ++i) {
        bind_results[i] = {};
        bind_results[i].buffer_type = MYSQL_TYPE_STRING;
        bind_results[i].buffer = result_buffers[i].data();
        bind_results[i].buffer_length = result_buffers[i].size();
        bind_results[i].length = &lengths[i];
    }
    if (mysql_stmt_bind_result(stmt, bind_results.data())) {
        std::cerr << "C_API Error: mysql_stmt_bind_result() failed: " << mysql_stmt_error(stmt) << std::endl;
    } else {
        while (mysql_stmt_fetch(stmt) == 0) {
            std::vector<std::string> row;
            for (int i = 0; i < num_fields; ++i) {
                row.push_back(std::string(result_buffers[i].data(), lengths[i]));
            }
            results.push_back(row);
        }
    }

    mysql_free_result(result_metadata);
    mysql_stmt_close(stmt);
    return true;
}