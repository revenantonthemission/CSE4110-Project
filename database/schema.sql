-- 테이블 생성 순서의 역순으로 테이블 삭제 (참조 무결성)
DROP TABLE IF EXISTS Transaction_Detail;
DROP TABLE IF EXISTS Transaction;
DROP TABLE IF EXISTS Store_Product;
DROP TABLE IF EXISTS Customer;
DROP TABLE IF EXISTS Product;
DROP TABLE IF EXISTS Vendor;
DROP TABLE IF EXISTS Store;

-- =================================================================
-- 테이블 생성
-- =================================================================

-- 1. 공급업체 정보 테이블
-- 제품을 공급하는 개별 업체들의 정보를 저장합니다.
CREATE TABLE Vendor (
    vendor_id           INT AUTO_INCREMENT,
    name                VARCHAR(100) NOT NULL UNIQUE,
    contact_person      VARCHAR(100),
    contact_email       VARCHAR(255) UNIQUE,
    address             VARCHAR(255),
    PRIMARY KEY (vendor_id)
);

-- 2. 매장 정보 테이블
-- 개별 편의점 지점의 기본 정보를 저장합니다.
CREATE TABLE Store (
    store_id            INT AUTO_INCREMENT,
    name                VARCHAR(100) NOT NULL,
    address             VARCHAR(255) UNIQUE,
    open_time           TIME,
    close_time          TIME,
    ownership_type      VARCHAR(20) CHECK (ownership_type IN ('Corporate', 'Franchise')),
    PRIMARY KEY (store_id)
);

-- 3. 상품 정보 테이블
-- 편의점에서 판매하는 모든 상품의 고유 정보를 저장합니다.
CREATE TABLE Product (
    product_upc         VARCHAR(13),
    name                VARCHAR(100) NOT NULL,
    brand               VARCHAR(100),
    price               DECIMAL(10, 2) CHECK (price >= 0),
    package_type        VARCHAR(50),
    size                VARCHAR(50),
    vendor_id           INT,
    PRIMARY KEY (product_upc),
    FOREIGN KEY (vendor_id) REFERENCES Vendor(vendor_id)
        ON DELETE SET NULL -- 공급업체가 삭제되어도 상품 정보는 남김
);

-- 4. 고객 정보 테이블
-- 멤버십 프로그램에 가입한 고객의 정보를 저장합니다.
CREATE TABLE Customer (
    customer_id         INT AUTO_INCREMENT,
    name                VARCHAR(100) NOT NULL,
    email               VARCHAR(255) UNIQUE,
    phone               VARCHAR(20),
    points              INT DEFAULT 0 CHECK (points >= 0),
    PRIMARY KEY (customer_id)
);

-- 5. 매장별 상품 재고 테이블 (M:N 관계)
-- 각 매장이 어떤 상품을 얼마나 보유하고 있는지 재고 정보를 저장합니다.
CREATE TABLE Store_Product (
    store_id            INT,
    product_upc         VARCHAR(13),
    inventory_level     INT NOT NULL DEFAULT 0 CHECK (inventory_level >= 0),
    reorder_threshold   INT,
    reorder_quantity    INT,
    PRIMARY KEY (store_id, product_upc),
    FOREIGN KEY (store_id) REFERENCES Store(store_id)
        ON DELETE CASCADE, -- 매장이 폐점하면 해당 매장의 재고 정보도 삭제
    FOREIGN KEY (product_upc) REFERENCES Product(product_upc)
        ON DELETE CASCADE -- 상품이 단종되면 모든 매장의 재고 정보에서 삭제
);

-- 6. 거래 정보 테이블
-- 한 번의 구매 행위에 대한 전체 정보를 요약하여 저장합니다.
CREATE TABLE Transaction (
    transaction_id      BIGINT AUTO_INCREMENT,
    store_id            INT,
    customer_id         INT, -- 비회원 구매 시 NULL
    transaction_time    TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    payment_method      VARCHAR(20) CHECK (payment_method IN ('Card', 'Cash', 'MobilePay')),
    total_amount        DECIMAL(10, 2) NOT NULL,
    PRIMARY KEY (transaction_id),
    FOREIGN KEY (store_id) REFERENCES Store(store_id)
        ON DELETE SET NULL, -- 매장이 폐점해도 거래 기록은 남김
    FOREIGN KEY (customer_id) REFERENCES Customer(customer_id)
        ON DELETE SET NULL -- 고객이 탈퇴해도 거래 기록은 남김
);

-- 7. 거래 상세 내역 테이블 (M:N 관계)
-- 한 거래에 어떤 상품이 몇 개, 얼마에 팔렸는지 상세 내역을 저장합니다.
CREATE TABLE Transaction_Detail (
    transaction_id      BIGINT,
    product_upc         VARCHAR(13),
    quantity            INT NOT NULL CHECK (quantity > 0),
    price_at_purchase   DECIMAL(10, 2) NOT NULL, -- 상품 가격 변동에 대비해 판매 시점 가격을 저장
    PRIMARY KEY (transaction_id, product_upc),
    FOREIGN KEY (transaction_id) REFERENCES Transaction(transaction_id)
        ON DELETE CASCADE, -- 주 거래가 삭제되면 상세 내역도 삭제
    FOREIGN KEY (product_upc) REFERENCES Product(product_upc)
		-- 상품이 삭제되는 것을 막음
        ON DELETE RESTRICT
);