package com.xgd.axxlab.ui.thanks;

import android.app.ActionBar;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.fragment.app.Fragment;
import androidx.lifecycle.ViewModelProvider;
import androidx.recyclerview.widget.RecyclerView;
import androidx.recyclerview.widget.StaggeredGridLayoutManager;

import com.xgd.axxlab.R;
import com.xgd.axxlab.adapter.LicenseAdapter;

public class ThanksFragment extends Fragment {
    ThanksViewModel thanksViewModel;

    public View onCreateView(@NonNull LayoutInflater inflater,
                             ViewGroup container, Bundle savedInstanceState) {
        thanksViewModel = new ViewModelProvider.AndroidViewModelFactory(
                getActivity().getApplication()).create(ThanksViewModel.class);
        thanksViewModel.initData(getContext());
        View root = inflater.inflate(R.layout.fragment_thanks,
                container, false);
        root.setOnClickListener(v -> {
            Toast.makeText(getContext(),"碎片被点击了",Toast.LENGTH_SHORT).show();
        });
        final RecyclerView recyclerView = root.findViewById(R.id.thanks_recycler_view);
        StaggeredGridLayoutManager layoutManager = new
                StaggeredGridLayoutManager(2, StaggeredGridLayoutManager.VERTICAL);
        recyclerView.setLayoutManager(layoutManager);
        thanksViewModel.getData().observe(getViewLifecycleOwner(), data -> {
            LicenseAdapter adapter = new LicenseAdapter(data);
            recyclerView.setAdapter(adapter);
        });
        return root;
    }
}
