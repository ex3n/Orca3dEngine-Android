package com.orca.orca3dengine_android;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.support.v7.app.ActionBarActivity;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.ListView;

import com.dd.plist.NSArray;
import com.dd.plist.NSDictionary;
import com.dd.plist.NSObject;
import com.dd.plist.NSString;
import com.dd.plist.PropertyListParser;

import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.List;


public class ViewList extends ActionBarActivity {
private Bundle m_bundle;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_view_list);
        String titleName = getIntent().getExtras().getString("Model Name");
        setTitle(titleName + " Views");
        m_bundle = new Bundle();
        m_bundle.putString("Model Name", titleName);
        String pathAndFile = getIntent().getExtras().getString("Model Name");
        m_bundle.putString("Path", pathAndFile);
        pathAndFile = pathAndFile + "/AnatomyInfo.plist";
        final ListView viewList = (ListView) findViewById(R.id.viewList);
        List<String> viewListStrings = new ArrayList<String>();
        InputStream is = null;
        try {
            is = getAssets().open(pathAndFile);
            try {
                NSDictionary rootDict = (NSDictionary) PropertyListParser.parse(is);
                NSObject[] views = ((NSArray) rootDict.objectForKey("Views")).getArray();
                for (NSObject dict : views) {
                    NSDictionary viewDict = (NSDictionary)dict;

                    NSString layerName = (NSString)viewDict.objectForKey("Name");
                    viewListStrings.add(layerName.toString());
//                    NSObject[] viewLayers = ((NSArray) viewDict.objectForKey("Layers")).getArray();
//
//                    for(NSObject layerDict : viewLayers)
//                    {
//                        NSDictionary viewLayerDict = (NSDictionary)layerDict;
//                        NSString submenuLayerName = (NSString)viewLayerDict.objectForKey("Name");
//                        viewListStrings.add(submenuLayerName.toString());
//                    }



                }

            }
            catch (Exception ex)
            {

            }

        } catch (IOException e) {
            e.printStackTrace();
        }
        ArrayAdapter<String> adapter;
        adapter = new ArrayAdapter<String>(this,android.R.layout.simple_list_item_1,android.R.id.text1,viewListStrings);
        viewList.setAdapter(adapter);

        viewList.setOnItemClickListener(new AdapterView.OnItemClickListener() {

            @Override
            public void onItemClick(AdapterView<?> parent, final View view,
                                    int position, long id) {
                final String item = (String) parent.getItemAtPosition(position);
                Intent intent = new Intent(ViewList.this,com.orca.orca3denginelibrarymodule.Orca3DEngineActivity.class);

                //intent.setClass(ViewList.this,com.orca.orca3denginelibrarymodule.Orca3DEngineActivity.class);

                m_bundle.putString("View Name", item);
                intent.putExtras(m_bundle);

                startActivity(intent);

            }

        });
    }



    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_view_list, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }
}
