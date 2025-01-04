// main.dart
import 'package:flutter/material.dart';
import 'login_screen.dart';
import 'register_screen.dart';
import 'root_screen.dart';

void main() {
  runApp(const MyApp());
}

class MyApp extends StatelessWidget {
  const MyApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      debugShowCheckedModeBanner: false,
      title: 'Flutter Login App',
      theme: ThemeData(
        primarySwatch: Colors.blue,
      ),
      initialRoute: LoginScreen.routeName,
      routes: {
        LoginScreen.routeName: (ctx) => const LoginScreen(),
        RegisterScreen.routeName: (ctx) => const RegisterScreen(),
        RootScreen.routeName: (ctx) => const RootScreen(),
        // Add other routes here if needed
      },
    );
  }
}
