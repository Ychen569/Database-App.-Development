--find_customer function
CREATE OR REPLACE PROCEDURE find_customer( e_customer_id IN number, found OUT number)
AS
e_count number;

BEGIN

SELECT count(cust_no)
INTO e_count
FROM customers
WHERE cust_no = e_customer_id; 

IF e_count > 0 THEN
found := 1;
END IF;
--DBMS_OUTPUT.PUT_LINE (found);
EXCEPTION 
    WHEN no_data_found THEN
    found := 0;
    WHEN others THEN
    
--DBMS_OUTPUT.PUT_LINE (found);   
DBMS_OUTPUT.PUT_LINE ('Error');
    
END find_customer;
/
Begin
    find_customer(&customerID);
END;

SET SERVEROUTPUT ON

--find_product function
CREATE OR REPLACE PROCEDURE find_product(product_id IN number)--, price OUT products.pro_sell%TYPE)
AS
price products.pro_sell%TYPE
BEGIN
    SELECT prod_sell
    INTO price
    FROM products
    WHERE prod_no = product_id;
EXCEPTION
WHEN no_data_found THEN
    	price := 0;
WHEN others THEN   
DBMS_OUTPUT.PUT_LINE ('Error');
    
END find_prodct; 

--add_order function
CREATE OR REPLACE PROCEDURE add_order(customer_id IN customers.cust_no%type,new_order_id OUT orders.order_no%type)
AS
--new_order_id orders.order_no %type;
BEGIN
    SELECT MAX(order_no)+1
    INTO new_order_id
    FROM orders;
    
    INSERT INTO 
    orders(order_no,rep_no,cust_no,order_dt,status)
    VALUES(new_order_id,36,customer_id,sysdate,'C');

EXCEPTION   
WHEN others THEN   
DBMS_OUTPUT.PUT_LINE ('Error');
END add_order;
/
BEGIN
 add_order(&customer_id);
END;

rollback;   

--add_orderline
CREATE OR REPLACE PROCEDURE add_orderline(orderId IN orderlines.order_no%type,
                                            itemId IN orderlines.line_no%type,
                                            productId IN orderlines.prod_no%type,
                                            quantity IN orderlines.qty%type,
                                            price IN orderlines.price%type)
AS
BEGIN
    INSERT INTO
    orderlines(order_no,line_no,orderlines.prod_no,qty,price)
    VALUES(orderId,itemId,productId,quantity,price);
EXCEPTION
WHEN others THEN   
DBMS_OUTPUT.PUT_LINE ('Error');
END add_orderline;
    
    
    
    
