import threading

orders_statuses = {}
orders_lock = threading.Lock()
