import 'package:firebase_core/firebase_core.dart';
import 'package:flutter/material.dart';
import 'login_screen.dart';
import 'register_screen.dart';
import 'root_screen.dart';
import 'profile_selection_screen.dart';
import 'settings_screen.dart';

void main() async {
  WidgetsFlutterBinding.ensureInitialized();
  await Firebase.initializeApp();
  runApp(const MyApp());
}

class MyApp extends StatelessWidget {
  const MyApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      debugShowCheckedModeBanner: false,
      title: 'Safe Step App',
      theme: ThemeData(primarySwatch: Colors.blue),
      initialRoute: LoginScreen.routeName,
      routes: {
        LoginScreen.routeName: (ctx) => const LoginScreen(),
        RegisterScreen.routeName: (ctx) => const RegisterScreen(),
        RootScreen.routeName: (ctx) => const RootScreen(),
        ProfileSelectionScreen.routeName: (ctx) => const ProfileSelectionScreen(),
        SettingsQuestionnaire.routeName: (ctx) => const SettingsQuestionnaire(),
      },
    );
  }
}