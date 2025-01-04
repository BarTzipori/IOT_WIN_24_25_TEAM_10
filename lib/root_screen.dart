import 'package:flutter/material.dart';
import 'settings_screen.dart'; // Import your settings screen here.

class RootScreen extends StatefulWidget {
  static const routeName = '/RootScreen';
  const RootScreen({Key? key}) : super(key: key);

  @override
  State<RootScreen> createState() => _RootScreenState();
}

class _RootScreenState extends State<RootScreen> {
  @override
  Widget build(BuildContext context) {
    return Scaffold(
      body: Center(
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: [
            Text(
              'Welcome to Step Safe',
              style: Theme.of(context).textTheme.headlineMedium?.copyWith(
                fontWeight: FontWeight.bold,
              ),
            ),
            const SizedBox(height: 24),
            ElevatedButton(
              onPressed: () {
                Navigator.push(
                  context,
                  MaterialPageRoute(
                    builder: (context) => const SettingsQuestionnaire(),
                  ),
                );
              },
              child: const Text('Settings'),
            ),
          ],
        ),
      ),
    );
  }
}
