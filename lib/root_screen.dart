import 'dart:developer';

import 'package:flutter/material.dart';

class RootScreen extends StatefulWidget {
  static const routeName = '/RootScreen';
  const RootScreen({super.key});

  @override
  State<RootScreen> createState() => _RootScreenState();
}

class _RootScreenState extends State<RootScreen> {
  late List<Widget> screens;
  int currentScreen = 0;
  late PageController controller;

  @override
  void initState() {
    super.initState();
    screens = const [
      HomeScreen(),
      SearchScreen(),
      CartScreen(),
      ProfileScreen(),
    ];
    controller = PageController(initialPage: currentScreen);
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      body: PageView(
        physics: const NeverScrollableScrollPhysics(),
        controller: controller,
        children: screens,
      ),
      bottomNavigationBar: NavigationBar(
        selectedIndex: currentScreen,
        backgroundColor: Theme.of(context).scaffoldBackgroundColor,
        elevation: 10,
        height: kBottomNavigationBarHeight,
        onDestinationSelected: (index) {
          setState(() {
            currentScreen = index;
          });
          controller.jumpToPage(currentScreen);
        },
        destinations: [
          const NavigationDestination(
            selectedIcon: Icon(Icons.home_filled),
            icon: Icon(Icons.home_outlined),
            label: "Home",
          ),
          const NavigationDestination(
            selectedIcon: Icon(Icons.search),
            icon: Icon(Icons.search_outlined),
            label: "Search",
          ),
          const NavigationDestination(
            selectedIcon: Icon(Icons.shopping_cart),
            icon: Icon(Icons.shopping_cart_outlined),
            label: "Cart",
          ),
          const NavigationDestination(
            selectedIcon: Icon(Icons.person),
            icon: Icon(Icons.person_outline),
            label: "Profile",
          ),
        ],
      ),
    );
  }
}

// Placeholder for HomeScreen
class HomeScreen extends StatelessWidget {
  const HomeScreen({super.key});

  @override
  Widget build(BuildContext context) {
    return Center(
      child: Text(
        'Home Screen',
        style: Theme.of(context).textTheme.headlineMedium,
      ),
    );
  }
}

// Placeholder for SearchScreen
class SearchScreen extends StatelessWidget {
  const SearchScreen({super.key});

  @override
  Widget build(BuildContext context) {
    return Center(
      child: Text(
        'Search Screen',
        style: Theme.of(context).textTheme.headlineMedium,
      ),
    );
  }
}

// Placeholder for CartScreen
class CartScreen extends StatelessWidget {
  const CartScreen({super.key});

  @override
  Widget build(BuildContext context) {
    return Center(
      child: Text(
        'Cart Screen',
        style: Theme.of(context).textTheme.headlineMedium,
      ),
    );
  }
}

// Placeholder for ProfileScreen
class ProfileScreen extends StatelessWidget {
  const ProfileScreen({super.key});

  @override
  Widget build(BuildContext context) {
    return Center(
      child: Text(
        'Profile Screen',
        style: Theme.of(context).textTheme.headlineMedium,
      ),
    );
  }
}
