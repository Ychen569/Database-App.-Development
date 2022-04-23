#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <cstring>
#include <occi.h>

using oracle::occi::Environment;
using oracle::occi::Connection;
using namespace std;
using namespace oracle::occi;

struct ShoppingCart
{
	int s_productId;
	double s_price;
	int s_quantity;
};

int mainMenu();
int customerLogin(Connection* conn, int customerId);
int addToCart(Connection* conn, struct ShoppingCart cart[]);
double findProduct(Connection* conn, int product_id);
void displayProducts(struct ShoppingCart cart[], int productCount);
int checkout(Connection* conn, struct ShoppingCart cart[], int customerId, int productCount);



int main()
{
	Environment* env = nullptr;
	Connection* conn = nullptr;

	string user = "dbs311_212c07";
	string pass = "22133228";
	string constr = "myoracle12c.senecacollege.ca:1521/oracle12c";


	try
	{
		env = Environment::createEnvironment(Environment::DEFAULT);
		conn = env->createConnection(user, pass, constr);
		cout << "Connection is Seccessful!" << endl << endl;
		
		int option = -1;
		int cust_id = 0;
		int numItems = 0;

		do
		{
			int option = mainMenu();
			if (option == 1)
			{
				cout << "Enter the customer ID: ";
				cin >> cust_id;
				cin.ignore(1000, '\n');

				if (customerLogin(conn, cust_id) == 1)
				{
					ShoppingCart cart[5]{};
					numItems = addToCart(conn, cart);
					displayProducts(cart, numItems);
					checkout(conn, cart, cust_id, numItems);

				}
				else
				{
					cout << "The customer does not exist." << endl;
				}
			}

		} while (option == 0);

		cout << "Thank you  --- Good bye..." << endl;

		env->terminateConnection(conn);
		Environment::terminateEnvironment(env);
	}
	catch (SQLException& sqlExcp)
	{
		cout << sqlExcp.getErrorCode() << ": " << sqlExcp.getMessage();
	}
	return 0;
}

int customerLogin(Connection* conn, int customerId)
{
	int validNo;
	try
	{
		Statement* stmt = conn->createStatement();
		stmt->setSQL("BEGIN find_customer(:1, :2); END; ");
		stmt->setInt(1, customerId);
		stmt->registerOutParam(2, Type::OCCIINT);
		stmt->executeUpdate();
		validNo = stmt->getInt(2);
		conn->terminateStatement(stmt);

	}
	catch (SQLException& sqlExcp)
	{
		cout << sqlExcp.getErrorCode() << ": " << sqlExcp.getMessage();
	}
	return validNo;
}

int mainMenu()
{
	int optionNo;
	bool valid = false;
	int customerId;
	cout << "***************Main Menu by XXXXXX*************" << endl;
	cout << "1) Login" << endl;
	cout << "0) Exit" << endl;
	cout << "Enter an option (0-1): ";
	while (!valid)
	{
		char str[81];
		cin >> optionNo;
		if (cin.fail())
		{
			cout << "Bad input, try again: ";
			cin.clear();
			cin.ignore(1000, '\n');
		}
		else
		{
			if (optionNo == 0 || optionNo == 1)
			{
				valid = true;
			}
			else cout << "You entered a wrong value. Enter an option(0-1): ";
		}
		cin.clear();
		cin.ignore(1000, '\n');
	};

	return optionNo;
}

double findProduct(Connection* conn, int productId)
{
	double price;
	try
	{
		Statement* stmt = conn->createStatement();
		stmt->setSQL("BEGIN find_product(:1, :2); END; ");
		stmt->setInt(1, productId);
		stmt->registerOutParam(2, Type::OCCIINT);
		stmt->executeUpdate();
		price = stmt->getInt(2);
		conn->terminateStatement(stmt);

	}
	catch (SQLException& sqlExcp)
	{
		cout << sqlExcp.getErrorCode() << ": " << sqlExcp.getMessage();
	}
	return price;

}

int addToCart(Connection* conn, struct ShoppingCart cart[])
{
	int productId;
	bool valid = false;
	double price;
	int count = 0;
	cout << "------------Add Products to Cart -----------------" << endl;
	while (!valid && count < 5)
	{
		cout << "Enter the product ID: ";
		cin >> productId;
		if (cin.fail())
		{
			cout << "Enter an integer number, try again: ";
			cin.clear();
			cin.ignore(1000, '\n');
		}
		if (findProduct(conn, productId) != 0)
		{
			cart[count].s_price = findProduct(conn, productId);
			cart[count].s_productId = productId;
			cout << "Product Price: " << cart[count].s_price << endl;
			cout << "Enter the product Quantity: ";
			cin >> cart[count].s_quantity;
			count++;

			if (count <= 5)
			{
				cout << "Enter 1 to add more products or 0 to checkout: ";
				int checkout;
				cin >> checkout;
				cin.clear();
				cin.ignore(1000, '\n');

				if (checkout == 0)
				{
					valid = true;
				}

			}
			else cout << "ShoppingCart is fulled, please checkout first" << endl;
		}
		else cout << "Invalid product, please check the product ID. ";
	}
	return count;
}

void displayProducts(struct ShoppingCart cart[], int productCount)
{
	double totalPrice = 0.0;
	cout << "------------Ordered Products----------------------------" << endl;
	for (int i = 0; i < productCount; i++)
	{
		cout << "------Item " << i + 1 << " Product ID : " << cart[i].s_productId << " Price : ";
		cout << cart[i].s_price << " Quantity : " << cart[i].s_quantity << endl;
		totalPrice += (cart[i].s_price * cart[i].s_quantity);
	}
	cout << "----------------------------------------------------------" << endl;
	cout << "Total : " << totalPrice << endl;
	cout << "==========================================================" << endl;
}

int checkout(Connection* conn, struct ShoppingCart cart[], int customerId, int productCount)
{
	int finished = 0;
	bool valid = false;
	char checkOut;
	
	cout << "Would you like to checkout? (Y/y or N/n)";
	while (!valid)
	{
		cin >> checkOut;
		if (cin.fail())
		{
			cout << "Enter a charactor, try again: ";
			cin.clear();
			cin.ignore(1000, '\n');
		}
		else 
		if (checkOut != 'Y' && checkOut != 'y' && checkOut != 'N' && checkOut != 'n')
		{
			cout << "Wrong input, please Enter Y/y or N/n";
		}
		else
		{
			valid = true;
		}
	}

	try
	{
		int order_no = 0;
		if (checkOut == 'Y' || checkOut == 'y')
		{
			Statement* stmt = conn->createStatement();
			stmt->setSQL("BEGIN add_order(:1, :2); END; ");
			stmt->setInt(1, customerId);
			stmt->registerOutParam(2, Type::OCCIINT);
			stmt->executeUpdate();
			order_no = stmt->getInt(2);
			/*conn->terminateStatement(stmt);*/
			conn-> rollback();

			for (int i = 0; i < productCount; i++)
			{
				stmt->setSQL("BEGIN add_orderline(:1, :2, :3, :4, :5); END; ");
				stmt->setInt(1, order_no);
				stmt->setInt(2, i+1);
				stmt->setInt(3, cart[i].s_productId);
				stmt->setInt(4, cart[i].s_quantity);
				stmt->setInt(5, cart[i].s_price);
				stmt->executeUpdate();

				conn->rollback();
			}
			cout << "The order is successful completed." << endl << endl;
			conn->terminateStatement(stmt);
			finished = 1;
		}
		else
		{
			cout << "The order is cancelled." << endl;
			for (int i = 0; i < productCount; i++)
			{
				cart[i].s_price = 0.0;
				cart[i].s_productId = 0;
				cart[i].s_quantity = 0;
			}
		}


	}
	catch (SQLException& sqlExcp){
		cout << sqlExcp.getErrorCode() << ": " << sqlExcp.getMessage();
	}

	return finished;
}




