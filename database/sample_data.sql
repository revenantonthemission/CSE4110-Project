
-- 데이터 삽입 전, 기존 데이터가 있다면 모두 삭제하여 초기 상태로 만듭니다.
-- (DELETE는 AUTO_INCREMENT 값을 초기화하지 않으므로, 완전히 새로 시작하려면 TRUNCATE TABLE을 사용하세요)
SET FOREIGN_KEY_CHECKS = 0; -- 외래 키 제약조건을 잠시 비활성화
TRUNCATE TABLE Transaction_Detail;
TRUNCATE TABLE Transaction;
TRUNCATE TABLE Store_Product;
TRUNCATE TABLE Customer;
TRUNCATE TABLE Product;
TRUNCATE TABLE Vendor;
TRUNCATE TABLE Store;
SET FOREIGN_KEY_CHECKS = 1; -- 외래 키 제약조건을 다시 활성화

-- =================================================================
-- 1. Vendor (공급업체) 데이터 삽입
-- =================================================================
INSERT INTO Vendor (vendor_id, name, contact_person, contact_email, address) VALUES
(1, '농심', '김농심', 'contact@nongshim.co.kr', '서울시 동작구 여의대방로 112'),
(2, '오리온', '박오리', 'master@orionworld.com', '서울시 용산구 백범로 90번길 13'),
(3, '코카-콜라 음료', '최콜라', 'help@ccbk.co.kr', '경기도 양주시 광적면 광적로 235'),
(4, '해태아이스크림', '이해태', 'haitai@htice.co.kr', '광주광역시 북구 하서로 325'),
(5, 'CJ제일제당', '강제당', 'cj@cj.net', '서울시 중구 동호로 330'),
(6, '유한킴벌리', '신킴벌', 'master@yuhan-kimberly.co.kr', '서울시 송파구 올림픽로 300');

-- =================================================================
-- 2. Store (매장) 데이터 삽입
-- =================================================================
INSERT INTO Store (store_id, name, address, open_time, close_time, ownership_type) VALUES
(1, 'CU 강남역점', '서울시 강남구 강남대로 390', '00:00:00', '23:59:59', 'Corporate'),
(2, 'GS25 신촌점', '서울시 서대문구 연세로 21', '07:00:00', '23:00:00', 'Franchise'),
(3, '7-Eleven 부산해운대점', '부산시 해운대구 구남로 28', '00:00:00', '23:59:59', 'Franchise');

-- =================================================================
-- 3. Customer (고객) 데이터 삽입
-- =================================================================
INSERT INTO Customer (customer_id, name, email, phone, points) VALUES
(1, '김민준', 'mj.kim@example.com', '010-1234-5678', 1500),
(2, '이서연', 'sy.lee@example.com', '010-2345-6789', 320),
(3, '박도윤', 'dy.park@example.com', '010-3456-7890', 8800),
(4, '최지우', 'jw.choi@example.com', '010-4567-8901', 50);

-- =================================================================
-- 4. Product (상품) 데이터 삽입
-- =================================================================
INSERT INTO Product (product_upc, name, brand, price, package_type, size, vendor_id) VALUES
('8801043015437', '신라면', '농심', 1200.00, '봉지', '120g', 1),
('8801062624890', '육개장사발면', '농심', 1000.00, '컵', '86g', 1),
('8801117135307', '코카-콜라', '코카-콜라', 2500.00, '페트', '500ml', 3),
('8801117211001', '스프라이트', '코카-콜라', 2400.00, '페트', '500ml', 3),
('8801115110107', '초코파이 정', '오리온', 5400.00, '박스', '12개입', 2),
('8801062237338', '새우깡', '농심', 1500.00, '봉지', '90g', 1),
('8801121151034', '부라보콘 화이트바닐라', '해태', 2000.00, '콘', '140ml', 4),
('8801007212046', '비비고 왕교자', 'CJ제일제당', 8900.00, '팩', '1005g', 5),
('8806002511475', '크리넥스 황사용마스크', '유한킴벌리', 3000.00, '개', '대형', 6);

-- =================================================================
-- 5. Store_Product (매장별 재고) 데이터 삽입
-- =================================================================
-- 1번 매장 (CU 강남역점) 재고
INSERT INTO Store_Product (store_id, product_upc, inventory_level, reorder_threshold, reorder_quantity) VALUES
(1, '8801043015437', 50, 20, 50), -- 신라면
(1, '8801062624890', 30, 15, 30), -- 육개장사발면
(1, '8801117135307', 100, 30, 100), -- 코카-콜라
(1, '8801115110107', 15, 10, 20), -- 초코파이
(1, '8806002511475', 25, 10, 30); -- 황사용마스크

-- 2번 매장 (GS25 신촌점) 재고
INSERT INTO Store_Product (store_id, product_upc, inventory_level, reorder_threshold, reorder_quantity) VALUES
(2, '8801043015437', 3, 10, 40), -- 신라면 (재주문 필요)
(2, '8801117135307', 80, 40, 80), -- 코카-콜라
(2, '8801117211001', 75, 40, 80), -- 스프라이트
(2, '8801062237338', 40, 20, 50), -- 새우깡
(2, '8801121151034', 30, 15, 40); -- 부라보콘

-- 3번 매장 (7-Eleven 부산해운대점) 재고
INSERT INTO Store_Product (store_id, product_upc, inventory_level, reorder_threshold, reorder_quantity) VALUES
(3, '8801117135307', 120, 50, 120), -- 코카-콜라
(3, '8801117211001', 110, 50, 120), -- 스프라이트
(3, '8801062237338', 5, 15, 50), -- 새우깡 (재주문 필요)
(3, '8801121151034', 45, 20, 50); -- 부라보콘

-- =================================================================
-- 6 & 7. Transaction (거래) 및 Transaction_Detail (거래 상세) 데이터 삽입
-- 거래 1: 고객 1이 1번 매장에서 구매
-- =================================================================
-- 먼저 주 거래 레코드를 생성 (total_amount는 나중에 업데이트)
INSERT INTO Transaction (store_id, customer_id, payment_method, total_amount) VALUES (1, 1, 'Card', 0);
SET @trans1 = LAST_INSERT_ID(); -- 방금 생성된 거래 ID를 변수에 저장

-- 거래 1의 상세 내역 추가
INSERT INTO Transaction_Detail (transaction_id, product_upc, quantity, price_at_purchase) VALUES
(@trans1, '8801043015437', 2, 1200.00), -- 신라면 2개
(@trans1, '8801117135307', 1, 2500.00); -- 코카-콜라 1개

-- 거래 1의 총액을 계산하여 업데이트
UPDATE Transaction SET total_amount = (2 * 1200.00 + 1 * 2500.00) WHERE transaction_id = @trans1;


-- =================================================================
-- 거래 2: 비회원 손님이 2번 매장에서 구매
-- =================================================================
INSERT INTO Transaction (store_id, customer_id, payment_method, total_amount) VALUES (2, NULL, 'Cash', 0);
SET @trans2 = LAST_INSERT_ID();

-- 거래 2의 상세 내역 추가
INSERT INTO Transaction_Detail (transaction_id, product_upc, quantity, price_at_purchase) VALUES
(@trans2, '8801062237338', 3, 1500.00); -- 새우깡 3개

-- 거래 2의 총액을 계산하여 업데이트
UPDATE Transaction SET total_amount = (3 * 1500.00) WHERE transaction_id = @trans2;


-- =================================================================
-- 거래 3: 고객 3이 1번 매장에서 대량 구매
-- =================================================================
INSERT INTO Transaction (store_id, customer_id, payment_method, total_amount) VALUES (1, 3, 'Card', 0);
SET @trans3 = LAST_INSERT_ID();

-- 거래 3의 상세 내역 추가
INSERT INTO Transaction_Detail (transaction_id, product_upc, quantity, price_at_purchase) VALUES
(@trans3, '8801115110107', 1, 5400.00), -- 초코파이 1박스
(@trans3, '8801117135307', 6, 2500.00); -- 코카-콜라 6개

-- 거래 3의 총액을 계산하여 업데이트
UPDATE Transaction SET total_amount = (1 * 5400.00 + 6 * 2500.00) WHERE transaction_id = @trans3;

-- =================================================================
-- 거래 4: 고객 2가 2번 매장에서 구매
-- =================================================================
INSERT INTO Transaction (store_id, customer_id, payment_method, total_amount) VALUES (2, 2, 'MobilePay', 0);
SET @trans4 = LAST_INSERT_ID();

-- 거래 4의 상세 내역 추가
INSERT INTO Transaction_Detail (transaction_id, product_upc, quantity, price_at_purchase) VALUES
(@trans4, '8801121151034', 2, 2000.00); -- 부라보콘 2개

-- 거래 4의 총액을 계산하여 업데이트
UPDATE Transaction SET total_amount = (2 * 2000.00) WHERE transaction_id = @trans4;

-- =================================================================
SELECT 'Sample data insertion complete.' AS 'Status';