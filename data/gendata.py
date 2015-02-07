import sys
import numpy.random as npr

f = open(sys.argv[1], 'w')

orderid = 100000
orderset = set()
type_dic = dict()
quantity_dic = dict()
price_dic = dict()

prob_add_sell = 0.4
prob_add_buy = 0.3
prob_delete = 0.2
prob_modify = 0.1

def quantity_price():
  quantity = npr.randint(1, 101) 
  price = npr.random() * 100
  return (quantity, price)

def add_sell():
  global orderid
  global type_dic
  global quantity_dic
  global price_dic
  (quantity, price) = quantity_price()
  f.write('A,' + str(orderid) + ',S,' + str(quantity) + ',' + str(price) + '\n')
  orderset.add(orderid)
  type_dic[orderid] = 'S'
  quantity_dic[orderid] = quantity
  price_dic[orderid] = price
  orderid += 1

def add_buy():
  global orderid
  global type_dic
  global quantity_dic
  global price_dic
  (quantity, price) = quantity_price()
  f.write('A,' + str(orderid) + ',B,' + str(quantity) + ',' + str(price) + '\n')
  orderset.add(orderid)
  type_dic[orderid] = 'B'
  quantity_dic[orderid] = quantity
  price_dic[orderid] = price
  orderid += 1

def delete():
  global type_dic
  global quantity_dic
  global price_dic
  global orderset
  if len(orderset) == 0:
    return
  orderid = int(npr.choice(list(orderset), 1))
  f.write('X,' + str(orderid) + ',' + str(type_dic[orderid]) + ',' + str(quantity_dic[orderid]) + ',' + str(price_dic[orderid]) + '\n')
  orderset.remove(orderid)

def modify():
  global type_dic
  global quantity_dic
  global price_dic
  global orderset
  if len(orderset) == 0:
    return
  orderid = int(npr.choice(list(orderset), 1))
  (quantity, price) = quantity_price()
  f.write('M,'+str(orderid) + ',' + str(type_dic[orderid]) + ',' + str(quantity) + ',' + str(price) + '\n')
  quantity_dic[orderid] = quantity
  price_dic[orderid] = price

iteration = 100000
for it in range(iteration):
  coin = npr.rand()
  if coin < prob_add_sell:
    add_sell()
  elif coin < prob_add_sell + prob_add_buy:
    add_buy()
  elif coin < prob_add_sell + prob_add_buy + prob_delete:
    delete()
  elif coin < prob_add_sell + prob_add_buy + prob_delete + prob_modify:
    modify()

f.close()






