import sys
import lele

MENU = [
    ("Truffle Mac & Cheese",      "Creamy cavatappi tossed in truffle-infused cheese sauce, finished with crispy panko.",         "$13.49", "foods/IMG_0232.png"),
    ("Buffalo Chicken Sandwich",  "Crispy chicken breast tossed in spicy buffalo sauce on a brioche bun with blue cheese slaw.",  "$11.99", "foods/IMG_0943.png"),
    ("Caprese Salad",             "Heirloom tomatoes, fresh mozzarella and basil drizzled with aged balsamic glaze.",             "$10.79", "foods/IMG_1668.png"),
    ("Wood-fired Margherita",     "Thin-crust pizza with San Marzano tomatoes, fresh mozzarella and basil.",                      "$12.99", "foods/IMG_1724.png"),
    ("Greek Bowl",                "Lemon-herb quinoa, kalamata olives, feta, cucumber and lemon-tahini dressing.",                "$11.49", "foods/IMG_2768.png"),
    ("Berry Croissant",           "Flaky butter croissant filled with seasonal berries and almond cream.",                        "$4.79",  "foods/IMG_2846.png"),
    ("Loaded Potato Soup",        "Slow-simmered potato soup with bacon, sharp cheddar and chives.",                              "$8.29",  "foods/IMG_3157.png"),
    ("Avocado Toast",             "Smashed avocado, pickled radish and microgreens on grilled sourdough.",                        "$9.49",  "foods/IMG_3315.png"),
    ("Iced Caramel Latte",        "Double-shot espresso and milk over ice, swirled with salted caramel.",                         "$5.29",  "foods/IMG_3318.png"),
    ("Roasted Tomato Bisque",     "Slow-roasted tomatoes blended with cream and fresh basil.",                                    "$7.99",  "foods/IMG_3760.png"),
    ("Steak Frites",              "Seared sirloin with herbed butter, served with hand-cut fries.",                               "$16.99", "foods/IMG_3773.png"),
    ("Cobb Salad",                "Romaine, bacon, blue cheese, hard-boiled egg, avocado and grilled chicken.",                   "$11.29", "foods/IMG_3950.png"),
    ("Lemon Tart",                "Buttery tart shell filled with tangy lemon curd, topped with toasted meringue.",               "$5.99",  "foods/IMG_4431.png"),
    ("Pad Thai Bowl",             "Rice noodles stir-fried with shrimp, peanuts, scallions and tamarind sauce.",                  "$13.79", "foods/IMG_4651.png"),
    ("Cold Brew",                 "House-brewed cold brew, smooth and slow-steeped for 18 hours.",                                "$4.59",  "foods/IMG_5514.png"),
    ("Steak Quesadilla",          "Grilled steak, melted Oaxaca cheese and roasted peppers in a crisped flour tortilla.",         "$13.49", "foods/IMG_5956.png"),
    ("Beet & Goat Cheese Salad",  "Roasted beets, candied walnuts and goat cheese over arugula.",                                 "$10.99", "foods/IMG_6133.png"),
    ("Chocolate Lava Cake",       "Warm chocolate cake with a molten dark-chocolate center and vanilla bean ice cream.",          "$6.79",  "foods/IMG_6314.png"),
    ("Lobster Roll",              "Maine lobster tossed in lemon butter on a toasted New England roll.",                          "$18.99", "foods/IMG_6580.png"),
    ("Mushroom Risotto",          "Carnaroli rice slow-cooked with wild mushrooms and aged parmesan.",                            "$14.49", "foods/IMG_7111.png"),
    ("Matcha Latte",              "Ceremonial-grade matcha whisked with steamed milk.",                                           "$5.49",  "foods/IMG_7240.png"),
    ("Caesar Salad",              "Crisp romaine, shaved parmesan, garlic croutons and creamy Caesar dressing.",                  "$9.79",  "foods/IMG_8337.png"),
    ("Cuban Sandwich",            "Slow-roasted pork, ham, swiss, pickles and mustard on pressed Cuban bread.",                   "$12.49", "foods/IMG_8340.png"),
    ("Bacon Cheese Burger",       "Half-pound prime patty, crispy bacon, aged cheddar and special sauce on a brioche bun.",       "$13.99", "foods/IMG_8641.png"),
    ("Cinnamon Roll",             "Warm rolled pastry swirled with cinnamon and topped with cream-cheese frosting.",              "$4.99",  "foods/IMG_9934.png"),
]

bag_count = 0


def update_bag_labels():
    text = f"Bag  {bag_count}"
    for label_id in ("/bag-grid", "/bag-detail"):
        obj = lele.getObjectById(label_id)
        if obj:
            obj.setText(text)


def on_card_clicked(event):
    if event.code != lele.Event().Type.EVENT_CLICKED:
        return
    idx = int(event.target.id.split("-", 1)[1])
    name, desc, price, src = MENU[idx]
    lele.getObjectById("/detail/img").setSrc(src)
    lele.getObjectById("/detail/name").setText(name)
    lele.getObjectById("/detail/desc").setText(desc)
    lele.getObjectById("/detail/price").setText(price)


def on_order_clicked(event):
    if event.code != lele.Event().Type.EVENT_CLICKED:
        return
    global bag_count
    bag_count += 1
    update_bag_labels()


def main():
    if not lele.loadConfig("restaurant-1.json"):
        sys.exit(1)

    for i in range(len(MENU)):
        lele.addEventHandler(f"card-{i}", on_card_clicked)
    lele.addEventHandler("/detail/order", on_order_clicked)

    update_bag_labels()

    while lele.handleEvents():
        pass


main()
